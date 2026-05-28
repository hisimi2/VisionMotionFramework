#pragma once
#include "EquipmentCore.h"
#include <chrono>
#include <string>

namespace OperationThread
{
    class PickPlaceSequenceBase : public EC::ISequenceExecutable
    {
    public:
        PickPlaceSequenceBase(int repeatCount = 0);
        virtual ~PickPlaceSequenceBase() override = default;

        void OnInitialize() override;
        bool OnPoll() override;
        void OnCleanup() override;
        void OnError(const std::string& errorMsg) override;

        int GetCurrentIteration() const;
        int GetSuccessCount() const;
        std::string GetLastError() const;

    protected:
        virtual bool HandleStep(int step) = 0;
        virtual int GetStepCount() const = 0;
        void MoveToNextStep();
        bool IsStepTimeout() const;
        void LogStep(const std::string& message);

        int m_repeatCount;
        int m_currentIteration;
        int m_successCount;
        int m_currentStep;
        long m_moveTimeoutMs;
        bool m_initialized;
        std::string m_lastError;
        std::chrono::steady_clock::time_point m_stepStartTime;

    private:
    };
}
