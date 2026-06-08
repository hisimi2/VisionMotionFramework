#include "stdafx.h"
#include "Orchestrator.h"
#include "RunController.h"
#include "Context.h"

#include "AsyncExecutor.h"

#include <memory>
#include <mutex>
#include <exception>

namespace VMF
{
    Orchestrator::Orchestrator()
        : m_pVatEngine()
    {
    }

    Orchestrator::~Orchestrator()
    {
        VatEnginePtr engineToStop;
        {
            std::lock_guard<std::mutex> guard(m_seqMutex);
            engineToStop = m_pVatEngine;
            m_pVatEngine.reset();
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
        if (m_pVatEngine)
        {
            return m_pVatEngine->GetRepository();
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
        return m_observers.erase(id) >0;
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

    VatContextPtr Orchestrator::CreateContext(const VisionEventHandlerPtr& vm, DataRepositoryPtr& repo)
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

    bool Orchestrator::StartSequenceSafe(SequenceStrategyPtr strategy)
    {
        if (!strategy)
        {
            return false;
        }

        m_pCurrentStrategy = strategy;

        if (m_pVatEngine)
        {
            m_pVatEngine->StopSequence();
            m_pVatEngine.reset();
        }

        SequenceBuilderPtr builder;
        DataRepositoryPtr repo;
        VisionEventHandlerPtr vm;

        try
        {
            builder = strategy->CreateBuilder();
            repo = strategy->CreateRepository();
            vm = strategy->CreateVisionProcessor();
        }
        catch (...)
        {
            m_pCurrentStrategy.reset();
            return false;
        }

        if (!builder || !vm || !repo)
        {
            m_pCurrentStrategy.reset();
            return false;
        }

        VatContextPtr ctx;
        try
        {
            ctx = CreateContext(vm, repo);
        }
        catch (...)
        {
            m_pCurrentStrategy.reset();
            return false;
        }

        if (!ctx)
        {
            m_pCurrentStrategy.reset();
            return false;
        }

        try
        {
            strategy->ConfigureParams(ctx);
        }
        catch (const std::exception& ex)
        {
            ctx->SetLastError(ex.what());
            m_pCurrentStrategy.reset();
            return false;
        }
        catch (...)
        {
            ctx->SetLastError("Unknown exception in ConfigureParams");
            m_pCurrentStrategy.reset();
            return false;
        }

        VatActuatorPtr actuator = strategy->GetActuator();

        try
        {
            m_pVatEngine = std::make_shared<RunController>(builder, ctx, actuator);
        }
        catch (const std::exception& ex)
        {
            ctx->SetLastError(ex.what());
            m_pVatEngine.reset();
            m_pCurrentStrategy.reset();
            return false;
        }
        catch (...)
        {
            ctx->SetLastError("Unknown exception creating RunController");
            m_pVatEngine.reset();
            m_pCurrentStrategy.reset();
            return false;
        }

        {
            AsyncExecutorPtr runner = std::make_shared<AsyncExecutor>();
            runner->SetResultSink(this);
            m_pVatEngine->SetRunner(runner);
        }

        std::string seqName = strategy->GetSequenceName();
        if (!m_pVatEngine->RunSequence(seqName))
        {
            m_pVatEngine->StopSequence();
            m_pVatEngine.reset();
            m_pCurrentStrategy.reset();
            return false;
        }

        return true;
    }

    void Orchestrator::StopSequence()
    {
        VatEnginePtr engineToStop;
        {
            std::lock_guard<std::mutex> guard(m_seqMutex);
            engineToStop = m_pVatEngine;
            m_pVatEngine.reset();
            m_pCurrentStrategy.reset();
        }

if (engineToStop)
        {
         engineToStop->StopSequence();
        }
    }

    // --- [직접 모드] 구현 ---

    void Orchestrator::SetVisionProcessor(VisionEventHandlerPtr vp)
    {
        std::lock_guard<std::mutex> guard(m_seqMutex);
        m_directVisionProcessor = vp;
    }

    VisionEventHandlerPtr Orchestrator::GetVisionProcessor() const
    {
        std::lock_guard<std::mutex> guard(m_seqMutex);

        // 상태머신 모드: RunController → Context → VisionProcessor
        if (m_pVatEngine)
        {
            auto ctx = m_pVatEngine->GetContext();
            if (ctx) return ctx->GetVisionProcessorInterface();
        }
        return m_directVisionProcessor;
    }

    void Orchestrator::SetDataRepository(DataRepositoryPtr repo)
    {
        std::lock_guard<std::mutex> guard(m_seqMutex);
        m_directDataRepository = repo;
    }

    VatContextPtr Orchestrator::GetOrCreateContext()
    {
        std::lock_guard<std::mutex> guard(m_seqMutex);

        // 상태머신 모드: RunController의 Context 반환
        if (m_pVatEngine)
        {
            auto ctx = m_pVatEngine->GetContext();
            if (ctx) return ctx;
        }

        // 직접 모드: 없으면 생성 (MemorySequenceStrategy와 동일 패턴)
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

    bool Orchestrator::ExecuteDirectVisionCommand(VatCommand cmd)
    {
        auto ctx = GetOrCreateContext();
        if (!ctx) return false;
        return ctx->ExecuteVisionCommand(cmd);
    }

    bool Orchestrator::ExecuteDirectVisionCommand(VatCommand cmd, const StringMap& params)
    {
        auto ctx = GetOrCreateContext();
        if (!ctx) return false;
        for (const auto& kv : params)
            ctx->SetVisionParamAs<std::string>(kv.first, kv.second);
        return ctx->ExecuteVisionCommand(cmd);
    }

} // namespace VMF
