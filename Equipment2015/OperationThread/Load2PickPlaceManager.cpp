#include "stdafx.h"
#include "Load2PickPlaceManager.h"

namespace OperationThread
{
    Load2PickPlaceManager::Load2PickPlaceManager()
        : m_parts(nullptr)
    {
        m_manager = std::make_shared<EC::SequenceManager>();
    }

    Load2PickPlaceManager::~Load2PickPlaceManager()
    {
        if (m_manager)
        {
         m_manager->Terminate();
        }
    }

    void Load2PickPlaceManager::Start(Load2Parts* parts, int repeatCount)
    {
        m_parts = parts;
        m_sequence = std::make_shared<Load2PickPlaceSequence>(parts, repeatCount);
        m_manager->Start(m_sequence);
    }

    void Load2PickPlaceManager::Pause()
    {
        if (m_manager)
        {
            m_manager->Pause();
        }
    }

    void Load2PickPlaceManager::Resume()
    {
        if (m_manager)
        {
            m_manager->Resume();
        }
    }

    void Load2PickPlaceManager::Terminate()
    {
        if (m_manager)
        {
            m_manager->Terminate();
        }
    }

    EC::SequenceState Load2PickPlaceManager::GetState() const
    {
        return m_manager ? m_manager->GetState() : EC::SequenceState::Stop;
    }

    std::string Load2PickPlaceManager::GetStateString() const
    {
        return m_manager ? m_manager->GetStateString() : "Unknown";
    }

    bool Load2PickPlaceManager::IsComplete() const
    {
        return m_manager ? m_manager->IsComplete() : true;
    }

    std::string Load2PickPlaceManager::GetLastError() const
    {
        return m_sequence ? m_sequence->GetLastError() : "";
    }

    void Load2PickPlaceManager::WaitForCompletion()
    {
        if (m_manager)
        {
            m_manager->WaitForCompletion();
        }
    }

    void Load2PickPlaceManager::SetPickPosition(double x, double z)
    {
        if (m_sequence)
        {
            m_sequence->SetPickPosition(x, z);
        }
    }

    void Load2PickPlaceManager::SetPlacePosition(double x, double z)
    {
        if (m_sequence)
        {
            m_sequence->SetPlacePosition(x, z);
        }
    }

    void Load2PickPlaceManager::SetSafeZ(double z)
    {
        if (m_sequence)
        {
            m_sequence->SetSafeZ(z);
        }
    }

    void Load2PickPlaceManager::SetMoveTimeout(long timeoutMs)
    {
        if (m_sequence)
        {
            m_sequence->SetMoveTimeout(timeoutMs);
        }
    }

    int Load2PickPlaceManager::GetCurrentIteration() const
    {
        return m_sequence ? m_sequence->GetCurrentIteration() : 0;
    }

    int Load2PickPlaceManager::GetSuccessCount() const
    {
        return m_sequence ? m_sequence->GetSuccessCount() : 0;
    }
}
