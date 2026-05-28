#include "stdafx.h"
#include "Load1RobotSequence.h"
#include <iostream>
#include <sstream>

namespace OperationThread
{
    Load1RobotSequence::Load1RobotSequence(LPVOID parts, int repeatCount)
        : PickPlaceSequenceBase(repeatCount)
        , m_parts((Load1Parts*)parts)
        , m_pickX(100.0), m_pickY(200.0), m_pickZ(-10.0)
        , m_placeX(300.0), m_placeY(150.0), m_placeZ(-12.0)
        , m_safeZ(0.0)
        , m_clampIndex(0)
        , m_vacuumIndex(0)
    {
    }

    Load1RobotSequence::~Load1RobotSequence()
    {
    }

    void Load1RobotSequence::OnInitialize()
    {
        if (!m_parts)
            throw std::runtime_error("Load1RobotSequence: Parts is null");
        PickPlaceSequenceBase::OnInitialize();
        LogStep("Load1RobotSequence initialized");
    }

    bool Load1RobotSequence::HandleStep(int step)
    {
        auto s = static_cast<PickPlaceStep>(step);
        switch (s)
        {
        case PickPlaceStep::RailOpen:
            return HandleRailOpen();
        case PickPlaceStep::MovePickPositionXY:
            return HandleMovePickPositionXY();
        case PickPlaceStep::PreciserDown:
            return HandlePreciserDown();
        case PickPlaceStep::MovePickPositionZ:
            return HandleMovePickPositionZ();
        case PickPlaceStep::ClampPick:
            return HandleClampPick();
        case PickPlaceStep::VacuumOn:
            return HandleVacuumOn();
        case PickPlaceStep::MoveSafeZAfterPick:
            return HandleMoveSafeZAfterPick();
        case PickPlaceStep::MovePlacePositionXY:
            return HandleMovePlacePositionXY();
        case PickPlaceStep::MovePlacePositionZ:
            return HandleMovePlacePositionZ();
        case PickPlaceStep::ReleasePlace:
            return HandleReleasePlace();
        case PickPlaceStep::BlowOn:
            return HandleBlowOn();
        case PickPlaceStep::MoveSafeZAfterPlace:
            return HandleMoveSafeZAfterPlace();
        case PickPlaceStep::CheckRepeat:
            return HandleCheckRepeat();
        case PickPlaceStep::Complete:
            return HandleComplete();
        default:
            throw std::runtime_error("Unknown step");
        }
    }



    void Load1RobotSequence::SetPickPosition(double x, double y, double z)
    {
        m_pickX = x;
        m_pickY = y;
        m_pickZ = z;
    }

    void Load1RobotSequence::SetPlacePosition(double x, double y, double z)
    {
        m_placeX = x;
        m_placeY = y;
        m_placeZ = z;
    }

    void Load1RobotSequence::SetSafeZ(double z)
    {
        m_safeZ = z;
    }

    void Load1RobotSequence::SetMoveTimeout(long timeoutMs)
    {
        m_moveTimeoutMs = timeoutMs;
    }

    void Load1RobotSequence::SetClampIndex(int index)
    {
        m_clampIndex = index;
    }

    void Load1RobotSequence::SetVacuumIndex(int index)
    {
        m_vacuumIndex = index;
    }



    // ============= 단계 처리 함수들 =============

    bool Load1RobotSequence::HandleRailOpen()
    {
        LogStep("HandleRailOpen");

        if (!m_parts->CylBuffer.isBackward())
        {
            m_parts->CylBuffer.backward(false);
        }

        return true;
    }

    bool Load1RobotSequence::HandleMovePickPositionXY()
    {
        LogStep("HandleMovePickPositionXY");

        m_parts->AxisX.Move(m_pickX);
        m_parts->AxisY.Move(m_pickY);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool Load1RobotSequence::HandlePreciserDown()
    {
        LogStep("HandlePreciserDown");

        // 모든 Setplate 실린더를 down
        for (auto& cyl : m_parts->CylSetplate)
            cyl.down(false);

        return true;
    }

    bool Load1RobotSequence::HandleMovePickPositionZ()
    {
        LogStep("HandleMovePickPositionZ");

        m_parts->AxisZ.Move(m_pickZ);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool Load1RobotSequence::HandleClampPick()
    {
        LogStep("HandleClampPick");

        if (m_clampIndex >= 0 && m_clampIndex < static_cast<int>(m_parts->CylTransfer.size()))
        {
            m_parts->CylTransfer[m_clampIndex].clamp(false);
        }

        return true;
    }

    bool Load1RobotSequence::HandleVacuumOn()
    {
        LogStep("HandleVacuumOn");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].vaccum(false);
        }

        return true;
    }

    bool Load1RobotSequence::HandleMoveSafeZAfterPick()
    {
        LogStep("HandleMoveSafeZAfterPick");

        m_parts->AxisZ.Move(m_safeZ);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool Load1RobotSequence::HandleMovePlacePositionXY()
    {
        LogStep("HandleMovePlacePositionXY");

        m_parts->AxisX.Move(m_placeX);
        m_parts->AxisY.Move(m_placeY);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool Load1RobotSequence::HandleMovePlacePositionZ()
    {
        LogStep("HandleMovePlacePositionZ");

        m_parts->AxisZ.Move(m_placeZ);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool Load1RobotSequence::HandleReleasePlace()
    {
        LogStep("HandleReleasePlace");

        if (m_clampIndex >= 0 && m_clampIndex < static_cast<int>(m_parts->CylTransfer.size()))
        {
            m_parts->CylTransfer[m_clampIndex].release(false);
        }

        return true;
    }

    bool Load1RobotSequence::HandleBlowOn()
    {
        LogStep("HandleBlowOn");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].blow(false);
        }

        return true;
    }

    bool Load1RobotSequence::HandleMoveSafeZAfterPlace()
    {
        LogStep("HandleMoveSafeZAfterPlace");

        m_parts->AxisZ.Move(m_safeZ);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool Load1RobotSequence::HandleCheckRepeat()
    {
        LogStep("HandleCheckRepeat");

        ++m_currentIteration;
        ++m_successCount;

        std::ostringstream oss;
        oss << "Completed iteration: " << m_currentIteration;
        LogStep(oss.str());

        // 반복 횟수 확인
        if (m_repeatCount == 0)
        {
            // 무한 반복
            m_currentStep = (int)PickPlaceStep::RailOpen;
            return false;
        }
        else if (m_currentIteration < m_repeatCount)
        {
            // 반복 계속
            m_currentStep = (int)PickPlaceStep::RailOpen;
            return false;
        }

        // 반복 완료
        return true;
    }

    bool Load1RobotSequence::HandleComplete()
    {
        LogStep("HandleComplete");
        return false; // 완료
    }



    void Load1RobotSequence::LogStep(const std::string& message)
    {
        std::cout << "[Load1PickPlace] " << message << std::endl;
    }
}
