#include "stdafx.h"
#include "Load1PickPlaceManager.h"

namespace OperationThread
{
    Load1PickPlaceManager::Load1PickPlaceManager()
        : m_manager(std::make_shared<EquipmentCore::SequenceManager>())
    {
    }

    Load1PickPlaceManager::~Load1PickPlaceManager()
    {
    }

    void Load1PickPlaceManager::Start(Load1Parts* parts, int repeatCount)
    {
        if (!parts)
            throw std::runtime_error("Load1PickPlaceManager: Load1Parts is null");

        // 어댑터 생성
        m_parts = parts;

        // 시퀀스 생성
        m_sequence = std::make_shared<Load1PickPlaceSequence>(
            parts,
            repeatCount
        );

        // 기본 파라미터 설정
        m_sequence->SetPickPosition(100.0, 200.0, -10.0);
        m_sequence->SetPlacePosition(300.0, 150.0, -12.0);
        m_sequence->SetSafeZ(0.0);
        m_sequence->SetMoveTimeout(3000);
        m_sequence->SetClampIndex(0);
        m_sequence->SetVacuumIndex(0);

        // 매니저 시작
        m_manager->Start(m_sequence);
    }

    void Load1PickPlaceManager::Pause()
    {
        m_manager->Pause();
    }

    void Load1PickPlaceManager::Resume()
    {
        m_manager->Resume();
    }

    void Load1PickPlaceManager::Terminate()
    {
        m_manager->Terminate();
    }

    EquipmentCore::SequenceState Load1PickPlaceManager::GetState() const
    {
        return m_manager->GetState();
    }

    std::string Load1PickPlaceManager::GetStateString() const
    {
        return m_manager->GetStateString();
    }

    bool Load1PickPlaceManager::IsComplete() const
    {
        return m_manager->IsComplete();
    }

    std::string Load1PickPlaceManager::GetLastError() const
    {
        return m_manager->GetLastError();
    }

    void Load1PickPlaceManager::WaitForCompletion()
    {
        m_manager->WaitForCompletion();
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
