#include "stdafx.h"
#include "SequenceStateMachine.h"

namespace EquipmentCore
{
    SequenceStateMachine::SequenceStateMachine()
        : m_currentState(SequenceState::Stop)
    {
    }

    SequenceStateMachine::~SequenceStateMachine()
    {
    }

    void SequenceStateMachine::Start()
    {
        ChangeState(SequenceState::Run);
    }

    void SequenceStateMachine::Pause()
    {
        ChangeState(SequenceState::Stop);
    }

    void SequenceStateMachine::Terminate()
    {
        ChangeState(SequenceState::Exit);
    }

    SequenceStateMachine::SequenceState SequenceStateMachine::GetCurrentState() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_currentState;
    }

    bool SequenceStateMachine::IsRunning() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_currentState == SequenceState::Run;
    }

    bool SequenceStateMachine::IsPaused() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_currentState == SequenceState::Stop;
    }

    bool SequenceStateMachine::IsTerminated() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_currentState == SequenceState::Exit;
    }

    void SequenceStateMachine::WaitForNotPaused()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [this]()
        {
            return m_currentState != SequenceState::Stop;
        });
    }

    void SequenceStateMachine::WaitStateChange(long timeoutMs)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        
        if (timeoutMs < 0)
        {
            m_condition.wait(lock);
        }
        else
        {
            m_condition.wait_for(lock, std::chrono::milliseconds(timeoutMs));
        }
    }

    void SequenceStateMachine::ChangeState(SequenceState newState)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_currentState = newState;
        }
        m_condition.notify_all();
    }
}
