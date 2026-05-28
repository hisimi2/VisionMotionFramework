#include "stdafx.h"
#include "Load2RobotManager.h"
#include "Load2RobotSequence.h"

namespace OperationThread
{
    Load2RobotManager::Load2RobotManager()
     : m_manager(std::make_shared<EC::SequenceManager>())
        , m_parts(nullptr)
    {
    }

 Load2RobotManager::~Load2RobotManager()
    {
  if (m_manager)
        {
   m_manager->Terminate();
        }
    }

   void Load2RobotManager::Start(Load2Parts* parts, int repeatCount)
  {
        m_parts = parts;
        m_sequence = std::make_shared<Load2RobotSequence>(parts, repeatCount);
        m_manager->Start(m_sequence);
    }

    void Load2RobotManager::Pause()
    {
  if (m_manager)
        {
         m_manager->Pause();
        }
    }

    void Load2RobotManager::Resume()
  {
        if (m_manager)
        {
     m_manager->Resume();
        }
    }

    void Load2RobotManager::Terminate()
    {
        if (m_manager)
        {
            m_manager->Terminate();
        }
    }

    std::string Load2RobotManager::GetStateString() const
    {
        return m_manager ? m_manager->GetStateString() : "Unknown";
    }

    bool Load2RobotManager::IsComplete() const
    {
      return m_manager ? m_manager->IsComplete() : true;
    }

    std::string Load2RobotManager::GetLastError() const
    {
    return m_sequence ? m_sequence->GetLastError() : "";
    }

    void Load2RobotManager::WaitForCompletion()
    {
      if (m_manager)
        {
  m_manager->WaitForCompletion();
        }
    }

    void Load2RobotManager::SetPickPosition(double x, double z)
    {
        if (m_sequence)
        {
            m_sequence->SetPickPosition(x, z);
        }
    }

    void Load2RobotManager::SetPlacePosition(double x, double z)
    {
      if (m_sequence)
{
            m_sequence->SetPlacePosition(x, z);
        }
    }

    void Load2RobotManager::SetSafeZ(double z)
    {
        if (m_sequence)
      {
      m_sequence->SetSafeZ(z);
        }
    }

    void Load2RobotManager::SetMoveTimeout(long timeoutMs)
    {
        if (m_sequence)
  {
       m_sequence->SetMoveTimeout(timeoutMs);
     }
    }

    int Load2RobotManager::GetCurrentIteration() const
    {
        return m_sequence ? m_sequence->GetCurrentIteration() : 0;
    }

    int Load2RobotManager::GetSuccessCount() const
    {
        return m_sequence ? m_sequence->GetSuccessCount() : 0;
    }
}
