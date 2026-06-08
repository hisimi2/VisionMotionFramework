#pragma once

#include "RunController.h"
#include "ISequenceStrategy.h"
#include "IResultSink.h"
#include "IVisionProcessor.h"
#include "IDataRepository.h"

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
        template <typename StrategyType>
        bool StartSequence(IActuator* adapter)
        {
            std::lock_guard<std::mutex> guard(m_seqMutex);
            SequenceStrategyPtr strategy = std::make_shared<StrategyType>();
            strategy->SetActuator(adapter);
            return StartSequenceSafe(strategy);
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
        /// 상태머신을 실행하지 않고 MemorySequenceStrategy의 컴포넌트 조립 로직을 재사용합니다.
        /// </summary>
        template <typename StrategyType>
        bool InitializeDirectWithStrategy(IActuator* actuator)
        {
            std::lock_guard<std::mutex> guard(m_seqMutex);

            auto strategy = std::make_shared<StrategyType>();
            strategy->SetActuator(actuator);

            // Strategy의 CreateRepository/CreateVisionProcessor 재사용
            DataRepositoryPtr repo;
            VisionEventHandlerPtr vp;
            try
            {
                repo = strategy->CreateRepository();
                vp = strategy->CreateVisionProcessor();
            }
            catch (...)
            {
                return false;
            }

            if (!repo || !vp) return false;

            m_directDataRepository = repo;
            m_directVisionProcessor = vp;

            // Context 생성 (CreateContext와 동일 패턴)
            m_directContext = std::make_shared<Context>();
            m_directContext->SetVisionProcessor(vp);
            m_directContext->SetDataRepository(repo);

            // Strategy의 ConfigureParams로 파라미터 설정
            // (MemorySequenceStrategy의 SetParam/AddVisionPoint 헬퍼 재사용)
            try
            {
                strategy->ConfigureParams(m_directContext);
            }
            catch (...)
            {
                m_directContext.reset();
                return false;
            }

            return true;
        }

protected:
        SequenceStrategyPtr m_pCurrentStrategy;
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
        bool StartSequenceSafe(SequenceStrategyPtr strategy);

        mutable std::mutex m_observerMutex;
        std::unordered_map<ObserverId, VisionResultObserver> m_observers;
        std::atomic<ObserverId> m_nextObserverId{1 };
    };
   
} // namespace VMF
