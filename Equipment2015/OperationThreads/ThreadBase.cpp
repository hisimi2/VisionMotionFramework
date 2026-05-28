#include "stdafx.h"
#include "ThreadBase.h"
#include <iostream>

namespace OperationThread
{
    ThreadBase::ThreadBase(int repeatCount)
        : m_repeatCount(repeatCount), m_currentIteration(0), m_successCount(0), m_currentStep(0),
          m_moveTimeoutMs(3000), m_initialized(false) {}

    void ThreadBase::OnInitialize()
    {
        m_initialized = true;
        m_currentIteration = 0;
        m_successCount = 0;
        m_currentStep = 0;
        LogStep("Sequence initialized");
    }

    bool ThreadBase::OnPoll()
    {
        if (!m_initialized) return false;
        try
        {
            bool stepComplete = HandleStep(m_currentStep);
            if (stepComplete && m_currentStep != GetStepCount() - 1)
            {
                MoveToNextStep();
            }
            return m_currentStep != GetStepCount() - 1;
        }
        catch (const std::exception& ex)
        {
            m_lastError = std::string("Poll error: ") + ex.what();
            throw;
        }
    }

    void ThreadBase::OnCleanup()
    {
        LogStep("Sequence cleanup completed");
    }

    void ThreadBase::OnError(const std::string& errorMsg)
    {
        m_lastError = errorMsg;
        std::cerr << "Sequence Error: " << errorMsg << std::endl;
    }

    int ThreadBase::GetCurrentIteration() const { return m_currentIteration; }
    int ThreadBase::GetSuccessCount() const { return m_successCount; }
    std::string ThreadBase::GetLastError() const { return m_lastError; }

    void ThreadBase::MoveToNextStep()
    {
        int nextStep = m_currentStep + 1;
        if (nextStep < GetStepCount())
        {
            m_currentStep = nextStep;
        }
    }

    bool ThreadBase::IsStepTimeout() const
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_stepStartTime).count();
        return elapsed > m_moveTimeoutMs;
    }

    void ThreadBase::LogStep(const std::string& message)
    {
        std::cout << "[ThreadBase] " << message << std::endl;
    }
}
