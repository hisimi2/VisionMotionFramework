#include "stdafx.h"
#include "SequenceManager.h"

namespace EC
{
    SequenceManager::SequenceManager(SequenceExecutablePtr sequence)
        : m_stateMachine(std::make_shared<SequenceStateMachine>())
        , m_executor(std::make_shared<SequenceExecutor>(m_stateMachine))
        , m_executable(sequence)
    {
    }

    SequenceManager::~SequenceManager()
    {
    }

    void SequenceManager::Start()
    {
        m_stateMachine->Start();
        m_executor->ExecuteAsync(m_executable);
    }

    void SequenceManager::Pause()
    {
        m_stateMachine->Pause();
    }

    void SequenceManager::Resume()
    {
        m_stateMachine->Start();
    }

    void SequenceManager::Terminate()
    {
        m_stateMachine->Terminate();
        m_executor->WaitForCompletion();
    }

    SequenceStateMachine::SequenceState SequenceManager::GetState() const
    {
        return m_stateMachine->GetCurrentState();
    }

    std::string SequenceManager::GetStateString() const
    {
        switch (m_stateMachine->GetCurrentState())
        {
            case SequenceStateMachine::SequenceState::Run:
                return "Run";
            case SequenceStateMachine::SequenceState::Stop:
                return "Stop";
            case SequenceStateMachine::SequenceState::Exit:
                return "Exit";
            default:
                return "Unknown";
        }
    }

    bool SequenceManager::IsComplete() const
    {
        return m_executor->IsExecutionComplete();
    }

    std::string SequenceManager::GetLastError() const
    {
        return m_executor->GetLastError();
    }

    void SequenceManager::WaitForCompletion()
    {
        m_executor->WaitForCompletion();
    }
}
