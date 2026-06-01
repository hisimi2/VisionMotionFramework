#pragma once

#include "ProcessController.h"
#include "IResultSink.h"

#include <mutex>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <atomic>
#include <cstdint>

namespace EC
{
    struct ResultPayload
    {
        int requestId;
        std::vector<std::string> results;
    };

    class EC_API Orchestrator : public IResultSink
    {
    public:
        using ObserverId = std::uint64_t;
        using ResultObserver = std::function<void(const ResultPayload& payload)>;

        Orchestrator();
        ~Orchestrator() override;

        // IResultSink 구현
        void NotifyResult(int requestId, const std::vector<std::string>& results) override;

        // ---- Observer API ----
        ObserverId AddObserver(ResultObserver observer);
        bool RemoveObserver(ObserverId id);
        void ClearObservers();

        // ---- Sequence control ----
        template <typename StrategyType>
        bool StartSequence()
        {
            std::lock_guard<std::mutex> guard(m_seqMutex);
            SequenceStrategyPtr strategy = std::make_shared<StrategyType>();
            strategy->SetActuator(adapter);
            return StartSequenceSafe(strategy);
        }

        void StopSequence();

    protected:
        ActivityBuilderPtr     m_pBuilder;
        ProcessControllerPtr   m_pProcess;

        virtual ContextPtr CreateContext();
        virtual void OnResult(int requestId, const std::vector<std::string>& results);
        void NotifyObservers(const ResultPayload& payload);

    private:
        mutable std::mutex m_seqMutex;
        bool StartSequenceSafe(ActivityBuilderPtr strategy);

        mutable std::mutex m_observerMutex;
        std::unordered_map<ObserverId, ResultObserver> m_observers;
        std::atomic<ObserverId> m_nextObserverId{1};
    };
} 
