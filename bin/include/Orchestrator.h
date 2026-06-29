#pragma once

#include "RunController.h"
#include "ISequenceSetup.h"
#include "IComponentSetup.h"
#include "ComponentSetupBase.h"
#include "IResultSink.h"
#include "IVisionProcessor.h"
#include "IDataRepository.h"
#include "AsyncExecutor.h"

#include <mutex>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <atomic>
#include <cstdint>

namespace VMF
{
    struct VisionResultPayload
    {
        int requestId;
        std::vector<std::string> results;
    };

    // VAT Sequence Orchestrator (UI 비종속).
    // - Strategy/Builder/Context/Runner 조립(Composition Root)
    // - VAT 시퀀스 실행/중지 API 제공
    // - Runner 결과를 받아 Observer에게 브로드캐스트
    class VMF_API Orchestrator : public IResultSink
    {
    public:
        using ObserverId = std::uint64_t;
        using VisionResultObserver = std::function<void(const VisionResultPayload& payload)>;

        Orchestrator();
        ~Orchestrator() override;

        // IResultSink 구현
        void NotifyVisionResult(int requestId, const std::vector<std::string>& results) override;

        // ---- Observer API ----
        ObserverId AddObserver(VisionResultObserver observer);
        bool RemoveObserver(ObserverId id);
        void ClearObservers();

        // ---- Sequence control ----
        /// <summary>
        /// StrategyType을 통해 VP/Repo/Context/Builder를 조립하고 상태머신(AsyncExecutor)으로 시퀀스를 실행합니다.
        /// </summary>
        template <typename StrategyType>
        bool StartSequence(IActuator* adapter)
        {
            std::lock_guard<std::mutex> guard(m_seqMutex);

            // 기존 엔진 중지
            if (m_pVisionEngine)
            {
                m_pVisionEngine->StopSequence();
                m_pVisionEngine.reset();
            }
            m_pCurrentStrategy.reset();

            return InitializeComponents<StrategyType>(adapter, true);
        }

        /// <summary>
        /// Vision 서버 연결 설정을 Strategy에 주입하여 시퀀스를 실행합니다.
        /// 동일 서버(IP:Port)에 대해서는 ConnectionManager가 단일 소켓을 공유합니다.
        /// </summary>
        /// <typeparam name="StrategyType">Strategy 타입</typeparam>
        /// <param name="adapter">액추에이터</param>
        /// <param name="connectionConfig">Vision 서버 연결 설정</param>
        template <typename StrategyType>
        bool StartSequence(IActuator* adapter, const VisionConnectionConfig& connectionConfig)
        {
            std::lock_guard<std::mutex> guard(m_seqMutex);

            // 기존 엔진 중지
            if (m_pVisionEngine)
            {
                m_pVisionEngine->StopSequence();
                m_pVisionEngine.reset();
            }
            m_pCurrentStrategy.reset();

            return InitializeComponentsWithConfig<StrategyType>(adapter, true, connectionConfig);
        }

        void StopSequence();

        // Repository accessor
        DataRepositoryPtr GetDataRepository();

        // --- [직접 모드] Strategy 없이 VisionProcessor/Repository/Context 사용 ---
        void SetVisionProcessor(VisionProcessorPtr vp);
        VisionProcessorPtr GetVisionProcessor() const;
        void SetDataRepository(DataRepositoryPtr repo);

        /// Context 획득 (직접 모드/상태머신 모드 모두 지원)
        VisionContextPtr GetOrCreateContext();

        /// 직접 비전 명령 실행 (상태머신 미사용)
        bool ExecuteDirectVisionCommand(VisionCommand cmd);
        bool ExecuteDirectVisionCommand(VisionCommand cmd, const StringMap& params);
        bool ExecuteDirectVisionCommand(VisionCommand cmd, const std::string& paramsName);

        /// <summary>
        /// Strategy를 통해 컴포넌트(Repository, VisionProcessor, Context)를 생성하고
        /// 파라미터를 설정하여 직접 모드를 초기화합니다.
        /// 상태머신을 실행하지 않고 Strategy의 컴포넌트 조립 로직을 재사용합니다.
        /// </summary>
        template <typename StrategyType>
        bool InitializeDirectWithStrategy(IActuator* actuator)
        {
            std::lock_guard<std::mutex> guard(m_seqMutex);
            return InitializeComponents<StrategyType>(actuator, false);
        }

        /// <summary>
        /// Vision 서버 연결 설정을 주입하여 직접 모드를 초기화합니다.
        /// </summary>
        template <typename StrategyType>
        bool InitializeDirectWithStrategy(IActuator* actuator, const VisionConnectionConfig& connectionConfig)
        {
            std::lock_guard<std::mutex> guard(m_seqMutex);
            return InitializeComponentsWithConfig<StrategyType>(actuator, false, connectionConfig);
        }

        /// <summary>
        /// IComponentSetup을 사용하여 직접 모드를 초기화합니다.
        /// CreateRepository, CreateVisionProcessor, ConfigureParams를 사용하여 VP/Repo/Context를 조립합니다.
        /// </summary>
        bool InitializeDirect(ComponentSetupPtr componentSetup)
        {
            std::lock_guard<std::mutex> guard(m_seqMutex);

            DataRepositoryPtr repo;
            VisionProcessorPtr vp;

            try
            {
                repo = componentSetup->CreateRepository();
                vp = componentSetup->CreateVisionProcessor();
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
                componentSetup->ConfigureParams(ctx);
            }
            catch (...)
            {
                return false;
            }

            m_directVisionProcessor = vp;
            m_directDataRepository = repo;
            m_directContext = ctx;

            return true;
        }

        /// <summary>
        /// [DLL Plugin] 외부 DLL에서 생성된 ComponentSetupBase(Strategy)를 받아 시퀀스를 실행합니다.
        /// Equipment 프로젝트가 VMFEquipmentPlugin.dll을 로드하여 CreateSetupStrategy()로
        /// 생성한 Strategy 객체를 전달하면, 컴포넌트를 초기화하고 시퀀스를 실행합니다.
        /// </summary>
        /// <param name="strategy">DLL에서 생성한 Strategy 객체 (ComponentSetupBase*)</param>
        /// <param name="actuator">액추에이터 (또는 nullptr)</param>
        /// <param name="connectionConfig">Vision 서버 연결 설정 (선택, 비워두면 기본 모드)</param>
        bool StartSequenceFromStrategy(
            std::shared_ptr<VMF::ComponentSetupBase> strategy,
            IActuator* actuator,
            const VisionConnectionConfig& connectionConfig = VisionConnectionConfig());

        /// <summary>
        /// [Direct Mode] 외부 DLL에서 생성된 ComponentSetupBase(Strategy)를 받아
        /// 직접 모드로 컴포넌트를 초기화합니다. (시퀀스 실행 없음)
        /// </summary>
        bool InitializeDirect(std::shared_ptr<VMF::ComponentSetupBase> strategy);

    protected:
        /// <summary>
        /// Strategy 기반 컴포넌트 조립을 통합 수행하는 공통 초기화 로직.
        /// connectionConfig가 nullptr이면 기본 모드, 값이 있으면 ConnectionManager 모드로 동작합니다.
        /// </summary>
        template <typename StrategyType>
        bool InitializeComponents(IActuator* actuator, bool runSequence,
            const VisionConnectionConfig* connectionConfig = nullptr)
        {
            auto strategy = std::make_shared<StrategyType>();
            strategy->SetActuator(actuator);

            if (connectionConfig)
            {
                strategy->SetConnectionConfig(*connectionConfig);
            }

            DataRepositoryPtr repo;
            VisionProcessorPtr vp;
            SequenceBuilderPtr builder;

            try
            {
                repo = strategy->CreateRepository();
                vp = strategy->CreateVisionProcessor();
                if (runSequence)
                {
                    builder = strategy->CreateBuilder();
                }
            }
            catch (...)
            {
                return false;
            }

            if (!repo || !vp) return false;
            if (runSequence && !builder) return false;

            auto ctx = CreateContext(vp, repo);
            if (!ctx) return false;

            try
            {
                strategy->ConfigureParams(ctx);
            }
            catch (...)
            {
                return false;
            }

            if (runSequence)
            {
                m_pCurrentStrategy = strategy;

                VisionActuatorPtr act = strategy->GetActuator();

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

                std::string seqName = strategy->GetSequenceName();
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
                m_directVisionProcessor = vp;
                m_directDataRepository = repo;
                m_directContext = ctx;
            }

            return true;
        }

        /// <summary>
        /// ConnectionConfig가 있는 버전의 InitializeComponents를 호출합니다.
        /// </summary>
        template <typename StrategyType>
        bool InitializeComponentsWithConfig(IActuator* actuator, bool runSequence,
            const VisionConnectionConfig& connectionConfig)
        {
            return InitializeComponents<StrategyType>(actuator, runSequence, &connectionConfig);
        }

        SequenceSetupPtr m_pCurrentStrategy;
        VisionEnginePtr m_pVisionEngine;

        // --- [직접 모드] Strategy 없이 VisionProcessor/Repository 직접 보관 ---
        VisionProcessorPtr m_directVisionProcessor;
        DataRepositoryPtr     m_directDataRepository;
        VisionContextPtr         m_directContext;

        virtual VisionContextPtr CreateContext(const VisionProcessorPtr& vm, DataRepositoryPtr& repo);

        // 기본 구현: Observer 통지
        virtual void OnVisionResult(int requestId, const std::vector<std::string>& results);

        void NotifyObservers(const VisionResultPayload& payload);

        private:
        mutable std::mutex m_seqMutex;

        mutable std::mutex m_observerMutex;
        std::unordered_map<ObserverId, VisionResultObserver> m_observers;
        std::atomic<ObserverId> m_nextObserverId{1 };
    };
   
} // namespace VMF
