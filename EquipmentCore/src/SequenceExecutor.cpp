#include "stdafx.h"
#include "SequenceExecutor.h"

namespace EC
{
    SequenceExecutor::SequenceExecutor(SequenceStateMachinePtr stateMachine)
        : m_stateMachine(stateMachine)
        , m_isComplete(false)
    {
    }

    SequenceExecutor::~SequenceExecutor()
    {
        if (m_executionThread.joinable())
        {
            m_executionThread.join();
        }
    }

    void SequenceExecutor::ExecuteAsync(SequenceExecutablePtr executable)
    {
        if (m_executionThread.joinable())
        {
            m_executionThread.join();
        }

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_isComplete = false;
            m_lastError.clear();
        }

        m_executionThread = std::thread(&SequenceExecutor::ExecuteSequenceThread, this, executable);
    }

    void SequenceExecutor::WaitForCompletion()
    {
        if (m_executionThread.joinable())
        {
            m_executionThread.join();
        }
    }

    bool SequenceExecutor::IsExecutionComplete() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_isComplete;
    }

    std::string SequenceExecutor::GetLastError() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_lastError;
    }

    void SequenceExecutor::ExecuteSequenceThread(SequenceExecutablePtr executable)
    {
        if (!executable)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_lastError = "SequenceExecutor: Invalid executable";
            m_isComplete = true;
            return;
        }

        try
        {
            executable->OnInitialize();

            // 시퀀스 실행 루프
            bool sequenceComplete = false;
            while (!sequenceComplete && m_stateMachine->GetCurrentState() != SequenceStateMachine::SequenceState::Exit)
            {
                // Stop 상태에서 대기
                if (m_stateMachine->GetCurrentState() == SequenceStateMachine::SequenceState::Stop)
                {
                    m_stateMachine->WaitForNotPaused();

                    // Exit 상태로 변경되었으면 종료
                    if (m_stateMachine->GetCurrentState() == SequenceStateMachine::SequenceState::Exit)
                    {
                        break;
                    }
                }

                // 시퀀스 폴링
                if (!executable->OnPoll())
                {
                    sequenceComplete = true;
                }

                // CPU 리소스 절약을 위한 짧은 대기
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            executable->OnCleanup();

            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_isComplete = true;
            }
        }
        catch (const std::exception& ex)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_lastError = std::string("SequenceExecutor: Exception - ") + ex.what();
            executable->OnError(m_lastError);
            m_isComplete = true;
        }
        catch (...)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_lastError = "SequenceExecutor: Unknown exception";
            executable->OnError(m_lastError);
            m_isComplete = true;
        }
    }
}
