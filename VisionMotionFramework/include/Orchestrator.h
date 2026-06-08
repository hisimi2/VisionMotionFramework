#pragma once

#include "RunController.h"
#include "ISequenceSetup.h"
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
            if (m_pVatEngine)
            {
                m_pVatEngine->StopSequence();
                m_pVatEngine.reset();
            }
            m_pCurrentStrategy.reset();

            return InitializeComponents<StrategyType>(adapter, true);
        }

        void StopSequence();

// Repository accessor
DataRepositoryPtr GetDataRepository();

        // --- [직접 모드] Strategy 없이 VisionProcessor/Repository/Context 사용 ---
        void SetVisionProcessor(VisionEventHandlerPtr vp);
        VisionEventHandlerPtr GetVisionProcessor() const;
        void SetDataRepository(DataRepositoryPtr repo);

        /// Context 획득 (직접 모드/상태머신 모드 모두 지원)
        VatContextPtr GetOrCreateContext();

/// 직접 비전 명령 실행 (상태머신 미사용)
        bool ExecuteDirectVisionCommand(VatCommand cmd);
        bool ExecuteDirectVisionCommand(VatCommand cmd, const StringMap& params);

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

protected:
        /// <summary>
        /// Strategy 기반 컴포넌트 조립을 통합 수행하는 공통 초기화 로직.
        /// - Strategy → VP, Repo, Builder 생성
        /// - CreateContext(vm, repo) → Context 연결
        /// - ConfigureParams(ctx) → 파라미터 주입
        /// - runSequence=true: 상태머신 모드 (RunController + AsyncExecutor)
        /// - runSequence=false: 직접 모드 (m_directXXX에 저장)
        /// </summary>
        template <typename StrategyType>
        bool InitializeComponents(IActuator* actuator, bool runSequence)
        {
            // -- Strategy 생성 --
            auto strategy = std::make_shared<StrategyType>();
            strategy->SetActuator(actuator);

            // -- Strategy가 VP, Repo, Builder 생성 --
            DataRepositoryPtr repo;
            VisionEventHandlerPtr vp;
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

            // -- Context 생성 및 VP/Repo 연결 --
            auto ctx = CreateContext(vp, repo);
            if (!ctx) return false;

            // -- Strategy가 파라미터 설정 --
            try
            {
                strategy->ConfigureParams(ctx);
            }
            catch (...)
            {
                return false;
            }

            // -- 모드별 분기 --
            if (runSequence)
            {
                // === 상태머신 모드 ===
                m_pCurrentStrategy = strategy;

                VatActuatorPtr act = strategy->GetActuator();

                try
                {
                    m_pVatEngine = std::make_shared<RunController>(builder, ctx, act);
                }
                catch (...)
                {
                    m_pCurrentStrategy.reset();
                    return false;
                }

                AsyncExecutorPtr runner = std::make_shared<AsyncExecutor>();
                runner->SetResultSink(this);
                m_pVatEngine->SetRunner(runner);

                std::string seqName = strategy->GetSequenceName();
                if (!m_pVatEngine->RunSequence(seqName))
                {
                    m_pVatEngine->StopSequence();
                    m_pVatEngine.reset();
                    m_pCurrentStrategy.reset();
                    return false;
                }
            }
            else
            {
                // === 직접 모드 ===
                m_directVisionProcessor = vp;
                m_directDataRepository = repo;
                m_directContext = ctx;
            }

            return true;
        }
        SequenceSetupPtr m_pCurrentStrategy;
        VatEnginePtr m_pVatEngine;

        // --- [직접 모드] Strategy 없이 VisionProcessor/Repository 직접 보관 ---
        VisionEventHandlerPtr m_directVisionProcessor;
        DataRepositoryPtr     m_directDataRepository;
        VatContextPtr         m_directContext;

        virtual VatContextPtr CreateContext(const VisionEventHandlerPtr& vm, DataRepositoryPtr& repo);

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
