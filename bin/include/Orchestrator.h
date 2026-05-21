#pragma once

#include "RunController.h"
#include "ISequenceStrategy.h"
#include "IResultSink.h"

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
        DataRepositoryPtr getDataRepository();

    protected:
        SequenceStrategyPtr m_pCurrentStrategy;
        VatEnginePtr m_pVatEngine;

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

    // Backward compatibility: keep the old names.
    // Prefer using `Orchestrator` in new code.
    using CVatEngineFacade = Orchestrator;
    using CVatEngineObserverAdapter = Orchestrator;
} // namespace VMF
