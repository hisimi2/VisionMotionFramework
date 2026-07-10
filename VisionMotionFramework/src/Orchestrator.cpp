#include "stdafx.h"
#include "Orchestrator.h"
#include "RunController.h"
#include "Context.h"
#include "AsyncExecutor.h"

#include <memory>
#include <mutex>

namespace VMF
{
Orchestrator::Orchestrator(std::shared_ptr<DefaultSetupStrategy> strategy,
                                IActuator* actuator,
                                const VisionConnectionConfig& connectionConfig)
        : m_pVisionEngine()
        , m_componentFactory(std::static_pointer_cast<IComponentSetup>(strategy))
        , m_sequenceFactory(std::static_pointer_cast<ISequenceSetup>(strategy))
    {
        // 생성 시점에 Actuator와 ConnectionConfig를 주입하고
        // Repository, VisionProcessor, Context를 미리 조립 (runSequence=false)
        bool hasConfig = (!connectionConfig.address.empty() && connectionConfig.port > 0);
        CreateComponentsAndRun(
            m_componentFactory.get(),
            actuator,
            hasConfig ? &connectionConfig : nullptr,
            strategy,   // presetStrategy
            false);     // runSequence = false (시퀀스는 별도 호출)
    }


    Orchestrator::~Orchestrator()
    {
        VisionEnginePtr engineToStop;
        {
            std::lock_guard<std::mutex> guard(m_seqMutex);
            engineToStop = m_pVisionEngine;
            m_pVisionEngine.reset();
            m_pCurrentStrategy.reset();
        }

        if (engineToStop)
        {
            engineToStop->StopSequence();
            engineToStop.reset();
        }

        ClearObservers();
    }

    DataRepositoryPtr Orchestrator::GetDataRepository()
    {
        std::lock_guard<std::mutex> guard(m_seqMutex);
        if (m_pVisionEngine)
        {
            return m_pVisionEngine->GetRepository();
        }
        return m_directDataRepository; // 직접 모드 지원
    }

    Orchestrator::ObserverId Orchestrator::AddObserver(VisionResultObserver observer)
    {
        if (!observer)
            return 0;

        const ObserverId id = m_nextObserverId.fetch_add(1);
        {
            std::lock_guard<std::mutex> lk(m_observerMutex);
            m_observers[id] = std::move(observer);
        }
        return id;
    }

    bool Orchestrator::RemoveObserver(ObserverId id)
    {
        std::lock_guard<std::mutex> lk(m_observerMutex);
        return m_observers.erase(id) > 0;
    }

    void Orchestrator::ClearObservers()
    {
        std::lock_guard<std::mutex> lk(m_observerMutex);
        m_observers.clear();
    }

    void Orchestrator::NotifyVisionResult(int requestId, const std::vector<std::string>& results)
    {
        OnVisionResult(requestId, results);
    }

    void Orchestrator::NotifyObservers(const VisionResultPayload& payload)
    {
        std::vector<VisionResultObserver> snapshot;
        {
            std::lock_guard<std::mutex> lk(m_observerMutex);
            snapshot.reserve(m_observers.size());
            for (auto& kv : m_observers)
            {
                if (kv.second)
                    snapshot.push_back(kv.second);
            }
        }

        for (auto& cb : snapshot)
        {
            try
            {
                cb(payload);
            }
            catch (...)
            {
                // ignore observer exceptions
            }
        }
    }

    VisionContextPtr Orchestrator::CreateContext(const VisionProcessorPtr& vm, DataRepositoryPtr& repo)
    {
        auto ctx = std::make_shared<Context>();
        ctx->SetVisionProcessor(vm);
        ctx->SetDataRepository(repo);
        return ctx;
    }

    void Orchestrator::OnVisionResult(int requestId, const std::vector<std::string>& results)
    {
        VisionResultPayload payload;
        payload.requestId = requestId;
        payload.results = results;
        NotifyObservers(payload);
    }

    void Orchestrator::StopSequence()
    {
        VisionEnginePtr engineToStop;
        {
            std::lock_guard<std::mutex> guard(m_seqMutex);
            engineToStop = m_pVisionEngine;
            m_pVisionEngine.reset();
            m_pCurrentStrategy.reset();
        }

        if (engineToStop)
        {
            engineToStop->StopSequence();
        }
    }

    // ============================================================================
    // 공통 컴포넌트 조립 로직 (리팩토링: 최종 통합 버전)
    // ============================================================================

    bool Orchestrator::CreateComponentsAndRun(IComponentSetup* factory,
                                              IActuator* actuator,
                                              const VisionConnectionConfig* connectionConfig,
                                              SequenceSetupPtr presetStrategy,
                                              bool runSequence,
                                              std::function<SequenceBuilderPtr()> builderFactory)
    {
        if (!factory)
            return false;

        // Actuator 설정 — nullptr이면 기존 설정 유지 (Strategy에 미리 주입된 값 보존)
        if (actuator != nullptr)
        {
            factory->SetActuator(actuator);
        }

        if (connectionConfig)
        {
            factory->SetConnectionConfig(*connectionConfig);
        }

        DataRepositoryPtr repo;
        VisionProcessorPtr vp;

        try
        {
            repo = factory->CreateRepository();
            vp = factory->CreateVisionProcessor();
        }
        catch (...)
        {
            return false;
        }

        if (!repo || !vp) return false;

        auto ctx = CreateContext(vp, repo);
        if (!ctx) return false;

        try
        {
            factory->ConfigureParams(ctx);
        }
        catch (...)
        {
            return false;
        }

        if (runSequence)
        {
            // 시퀀스 모드: Builder 필요
            if (!builderFactory)
                return false;

            SequenceBuilderPtr builder = builderFactory();
            if (!builder) return false;

            // m_pCurrentStrategy 저장 (preset 조회용)
            m_pCurrentStrategy = presetStrategy;

            VisionActuatorPtr act = factory->GetActuator();

            try
            {
                m_pVisionEngine = std::make_shared<RunController>(builder, ctx, act);
            }
            catch (...)
            {
                m_pCurrentStrategy.reset();
                return false;
            }

            AsyncExecutorPtr runner = std::make_shared<AsyncExecutor>();
            runner->SetResultSink(this);
            m_pVisionEngine->SetRunner(runner);

            std::string seqName = presetStrategy ? presetStrategy->GetSequenceName() : "";
            if (!m_pVisionEngine->RunSequence(seqName))
            {
                m_pVisionEngine->StopSequence();
                m_pVisionEngine.reset();
                m_pCurrentStrategy.reset();
                return false;
            }
        }
        else
        {
            // 직접 모드: VP/Repo/Context만 저장
            m_directVisionProcessor = vp;
            m_directDataRepository = repo;
            m_directContext = ctx;
            m_pCurrentStrategy = presetStrategy;  // [버그 수정] 누락된 m_pCurrentStrategy 저장
        }

        return true;
    }

    // ============================================================================
    // InitializeComponents (기존 유지보수 호환용 — CreateComponentsAndRun 호출)
    // ============================================================================

    bool Orchestrator::InitializeComponents(IComponentSetup* factory, IActuator* actuator,
                                             bool runSequence,
                                             const VisionConnectionConfig* connectionConfig)
    {
        if (runSequence)
        {
            return CreateComponentsAndRun(
                factory, actuator, connectionConfig,
                m_sequenceFactory,   // presetStrategy = m_sequenceFactory
                true,
                [this]() { return m_sequenceFactory ? m_sequenceFactory->CreateBuilder() : nullptr; });
        }
        else
        {
            return CreateComponentsAndRun(
                factory, actuator, connectionConfig,
                nullptr,   // presetStrategy = nullptr
                false);
        }
    }

    // ============================================================================
    // RunSequence (팩토리 기반)
    // ============================================================================

bool Orchestrator::RunSequence(IActuator* actuator,
                                    const VisionConnectionConfig& connectionConfig)
    {
        std::lock_guard<std::mutex> guard(m_seqMutex);

        // 기존 엔진 중지
        if (m_pVisionEngine)
        {
            m_pVisionEngine->StopSequence();
            m_pVisionEngine.reset();
        }
        m_pCurrentStrategy.reset();

        if (!m_componentFactory || !m_sequenceFactory)
            return false;

        bool hasConfig = (!connectionConfig.address.empty() && connectionConfig.port > 0);
        return InitializeComponents(
            m_componentFactory.get(),
            actuator,
            true,
            hasConfig ? &connectionConfig : nullptr);
    }

    // ============================================================================
    // RunSequence (무인자) — 생성자에서 이미 Actuator/Config가 주입된 경우 사용
    // ============================================================================

    bool Orchestrator::RunSequence()
    {
        std::lock_guard<std::mutex> guard(m_seqMutex);

        // 기존 엔진 중지
        if (m_pVisionEngine)
        {
            m_pVisionEngine->StopSequence();
            m_pVisionEngine.reset();
        }
        m_pCurrentStrategy.reset();

        if (!m_componentFactory || !m_sequenceFactory)
            return false;

        // 생성자에서 이미 Actuator와 ConnectionConfig가 주입되어 있으므로
        // InitializeComponents에 actuator=nullptr, connectionConfig=nullptr 전달
        // → CreateComponentsAndRun 내부에서 nullptr 체크에 따라 기존 값 유지
        return InitializeComponents(
            m_componentFactory.get(),
            nullptr,    // actuator = nullptr → 기존 주입값 유지
            true,       // runSequence = true
            nullptr);   // connectionConfig = nullptr → 기존 주입값 유지
    }

    VisionProcessorPtr Orchestrator::GetVisionProcessor() const
    {
        std::lock_guard<std::mutex> guard(m_seqMutex);

        // 상태머신 모드: RunController → Context → VisionProcessor
        if (m_pVisionEngine)
        {
            auto ctx = m_pVisionEngine->GetContext();
            if (ctx) return ctx->GetVisionProcessorInterface();
        }
        return m_directVisionProcessor;
    }

    VisionContextPtr Orchestrator::GetOrCreateContext()
    {
        std::lock_guard<std::mutex> guard(m_seqMutex);

        // 상태머신 모드: RunController의 Context 반환
        if (m_pVisionEngine)
        {
            auto ctx = m_pVisionEngine->GetContext();
            if (ctx) return ctx;
        }

        // 직접 모드: 없으면 생성
        if (!m_directContext)
        {
            m_directContext = std::make_shared<Context>();
            if (m_directVisionProcessor)
                m_directContext->SetVisionProcessor(m_directVisionProcessor);
            if (m_directDataRepository)
                m_directContext->SetDataRepository(m_directDataRepository);
        }
        return m_directContext;
    }

    bool Orchestrator::ExecuteDirectVisionCommand(VisionCommand cmd)
    {
        auto ctx = GetOrCreateContext();
        if (!ctx) return false;
        return ctx->ExecuteVisionCommand(cmd);
    }

bool Orchestrator::ExecuteDirectVisionCommand(VisionCommand cmd, const StringMap& params)
    {
        auto ctx = GetOrCreateContext();
        if (!ctx) return false;
        return ctx->ExecuteVisionCommand(cmd);
    }

    bool Orchestrator::ExecuteDirectVisionCommand(VisionCommand cmd, const std::string& presetName)
    {
        if (!m_pCurrentStrategy) return false;

        // Strategy에서 Preset 조회
        StringMap params = m_pCurrentStrategy->GetVisionParams(presetName);
        if (params.empty()) return false;

        // 기존 (cmd, params) 오버로드로 위임
        return ExecuteDirectVisionCommand(cmd, params);
    }

} // namespace VMF
