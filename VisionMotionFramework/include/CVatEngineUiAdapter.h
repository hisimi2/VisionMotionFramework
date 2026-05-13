#pragma once

#include "VatCorrectionEngine.h"
#include "IVatSequenceStrategy.h"
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

    // Engine/Host adapter (UI 비종속).
    // Runner 결과를 받아 Observer에게 전달하고, Strategy/Builder/Context/Runner 조립을 수행합니다.
    class VMF_API CVatEngineObserverAdapter : public IResultSink
    {
    public:
        using ObserverId = std::uint64_t;
        using VisionResultObserver = std::function<void(const VisionResultPayload& payload)>;

        CVatEngineObserverAdapter();
        ~CVatEngineObserverAdapter() override;

        // IResultSink 구현
        void NotifyVisionResult(int requestId, const std::vector<std::string>& results) override;

        // ---- Observer API ----
        ObserverId AddObserver(VisionResultObserver observer);
        bool RemoveObserver(ObserverId id);
        void ClearObservers();

        // ---- Sequence control ----
        template <typename StrategyType>
        bool StartVatSequence(IVatActuator* adapter)
        {
            std::lock_guard<std::mutex> guard(m_seqMutex);
            VatSequenceStrategyPtr strategy = std::make_shared<StrategyType>();
            strategy->SetActuator(adapter);
            return StartVatSequenceSafe(strategy);
        }

        void StopVatSequence();

        // Repository accessor
        DataRepositoryPtr getDataRepository();

    protected:
        VatSequenceStrategyPtr m_pCurrentStrategy;
        VatEnginePtr m_pVatEngine;

        virtual VatContextPtr CreateContext(const VisionEventHandlerPtr& vm, DataRepositoryPtr& repo);

        // 기본 구현: Observer 통지
        virtual void OnVisionResult(int requestId, const std::vector<std::string>& results);

        void NotifyObservers(const VisionResultPayload& payload);

    private:
        mutable std::mutex m_seqMutex;
        bool StartVatSequenceSafe(VatSequenceStrategyPtr strategy);

        mutable std::mutex m_observerMutex;
        std::unordered_map<ObserverId, VisionResultObserver> m_observers;
        std::atomic<ObserverId> m_nextObserverId{1 };
    };

    // Backward-compatibility: old name kept as alias.
    // (Consider removing after downstream code migration.)
    using CVatEngineUiAdapter = CVatEngineObserverAdapter;
} // namespace VMF
