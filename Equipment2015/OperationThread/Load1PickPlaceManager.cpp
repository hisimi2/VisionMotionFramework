#include "stdafx.h"
#include "Load1PickPlaceManager.h"
#include "Load1PickPlaceSequence.h"

namespace OperationThread
{
    Load1PickPlaceManager::Load1PickPlaceManager()
        : m_manager(std::make_shared<EC::SequenceManager>())
    , m_parts(nullptr)
    {
    }

    Load1PickPlaceManager::~Load1PickPlaceManager()
    {
        if (m_manager)
        {
      m_manager->Terminate();
        }
}

    void Load1PickPlaceManager::Start(Load1Parts* parts, int repeatCount)
    {
        if (!parts)
      throw std::runtime_error("Load1PickPlaceManager: Load1Parts is null");

        m_parts = parts;
        m_sequence = std::make_shared<Load1PickPlaceSequence>(parts, repeatCount);

        m_sequence->SetPickPosition(100.0, 200.0, -10.0);
        m_sequence->SetPlacePosition(300.0, 150.0, -12.0);
        m_sequence->SetSafeZ(0.0);
   m_sequence->SetMoveTimeout(3000);
        m_sequence->SetClampIndex(0);
        m_sequence->SetVacuumIndex(0);

        m_manager->Start(m_sequence);
    }

    void Load1PickPlaceManager::Pause()
    {
      if (m_manager)
        {
            m_manager->Pause();
        }
    }

    void Load1PickPlaceManager::Resume()
    {
        if (m_manager)
        {
       m_manager->Resume();
 }
    }

    void Load1PickPlaceManager::Terminate()
    {
        if (m_manager)
        {
  m_manager->Terminate();
    }
    }

    std::string Load1PickPlaceManager::GetStateString() const
    {
    return m_manager ? m_manager->GetStateString() : "Unknown";
    }

    bool Load1PickPlaceManager::IsComplete() const
    {
        return m_manager ? m_manager->IsComplete() : true;
    }

    std::string Load1PickPlaceManager::GetLastError() const
    {
        return m_manager ? m_manager->GetLastError() : "";
    }

    void Load1PickPlaceManager::WaitForCompletion()
    {
    if (m_manager)
        {
   m_manager->WaitForCompletion();
        }
    }

    void Load1PickPlaceManager::SetPickPosition(double x, double y, double z)
    {
        if (m_sequence)
      m_sequence->SetPickPosition(x, y, z);
    }

    void Load1PickPlaceManager::SetPlacePosition(double x, double y, double z)
    {
  if (m_sequence)
            m_sequence->SetPlacePosition(x, y, z);
    }

    void Load1PickPlaceManager::SetSafeZ(double z)
    {
        if (m_sequence)
            m_sequence->SetSafeZ(z);
}

    void Load1PickPlaceManager::SetMoveTimeout(long timeoutMs)
    {
      if (m_sequence)
     m_sequence->SetMoveTimeout(timeoutMs);
    }

    int Load1PickPlaceManager::GetCurrentIteration() const
    {
        return m_sequence ? m_sequence->GetCurrentIteration() : 0;
    }

    int Load1PickPlaceManager::GetSuccessCount() const
    {
     return m_sequence ? m_sequence->GetSuccessCount() : 0;
    }
}
