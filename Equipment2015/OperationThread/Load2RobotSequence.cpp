#include "stdafx.h"
#include "Load2RobotSequence.h"
#include <iostream>
#include <sstream>
#include "Actuators\Load2Parts.h"

namespace OperationThread
{
    Load2RobotSequence::Load2RobotSequence(Load2Parts* parts, int repeatCount)
        : m_parts(parts)
        , m_repeatCount(repeatCount)
        , m_currentIteration(0)
        , m_successCount(0)
        , m_currentStep(PickPlaceStep::RailOpen)
        , m_moveTimeoutMs(3000)
        , m_pickX(100.0), m_pickZ(-10.0)
        , m_placeX(300.0), m_placeZ(-12.0)
        , m_safeZ(0.0)
        , m_vacuumIndex(0)
        , m_initialized(false)
    {
    }

    Load2RobotSequence::~Load2RobotSequence()
    {
    }

    void Load2RobotSequence::OnInitialize()
    {
        if (!m_parts)
        throw std::runtime_error("Load2RobotSequence: Parts is null");

        m_initialized = true;
        m_currentIteration = 0;
        m_successCount = 0;
        m_currentStep = PickPlaceStep::RailOpen;

        LogStep("Load2RobotSequence initialized");
    }

    bool Load2RobotSequence::OnPoll()
    {
        if (!m_initialized)
            return false;

        try
        {
            bool stepComplete = false;

            // 현재 단계 처리
            switch (m_currentStep)
            {
                case PickPlaceStep::RailOpen:
                    stepComplete = HandleRailOpen();
                    break;
                case PickPlaceStep::MovePickPositionXZ:
                    stepComplete = HandleMovePickPositionXZ();
                    break;
                case PickPlaceStep::PreciserDown:
                    stepComplete = HandlePreciserDown();
                    break;
                case PickPlaceStep::VacuumOn:
                    stepComplete = HandleVacuumOn();
                    break;
                case PickPlaceStep::MoveSafeZAfterPick:
                    stepComplete = HandleMoveSafeZAfterPick();
                    break;
                case PickPlaceStep::MovePlacePositionXZ:
                    stepComplete = HandleMovePlacePositionXZ();
                    break;
                case PickPlaceStep::PreciserUp:
                    stepComplete = HandlePreciserUp();
                    break;
                case PickPlaceStep::VacuumOff:
                    stepComplete = HandleVacuumOff();
                    break;
                case PickPlaceStep::MoveSafeZAfterPlace:
                    stepComplete = HandleMoveSafeZAfterPlace();
                    break;
                case PickPlaceStep::CheckRepeat:
                    stepComplete = HandleCheckRepeat();
                    break;
                case PickPlaceStep::Complete:
                    stepComplete = HandleComplete();
                    break;
                default:
                    throw std::runtime_error("Unknown step");
            }

            if (stepComplete && m_currentStep != PickPlaceStep::Complete)
            {
                MoveToNextStep();
            }

            // false이면 시퀀스 완료
            return m_currentStep != PickPlaceStep::Complete;
        }
        catch (const std::exception& ex)
        {
            m_lastError = std::string("Poll error: ") + ex.what();
            throw;
        }
    }

    void Load2RobotSequence::OnCleanup()
    {
        LogStep("Load2RobotSequence cleanup completed");
    }

    void Load2RobotSequence::OnError(const std::string& errorMsg)
    {
        m_lastError = errorMsg;
        std::cerr << "Load2RobotSequence Error: " << errorMsg << std::endl;
    }

    void Load2RobotSequence::SetPickPosition(double x, double z)
    {
        m_pickX = x;
        m_pickZ = z;
    }

    void Load2RobotSequence::SetPlacePosition(double x, double z)
    {
        m_placeX = x;
        m_placeZ = z;
    }

    void Load2RobotSequence::SetSafeZ(double z)
    {
        m_safeZ = z;
    }

    void Load2RobotSequence::SetMoveTimeout(long timeoutMs)
    {
        m_moveTimeoutMs = timeoutMs;
    }

    void Load2RobotSequence::SetVacuumIndex(int index)
    {
        m_vacuumIndex = index;
    }

    Load2RobotSequence::PickPlaceStep Load2RobotSequence::GetCurrentStep() const
    {
        return m_currentStep;
    }

    int Load2RobotSequence::GetCurrentIteration() const
    {
        return m_currentIteration;
    }

    int Load2RobotSequence::GetSuccessCount() const
    {
        return m_successCount;
    }

    // ============= 단계 처리 함수들 =============
    bool Load2RobotSequence::HandleRailOpen()
    {
        LogStep("HandleRailOpen");

        if (!m_parts->CylRail.isOpen())
        {
            m_parts->CylRail.open(false);
        }

        return true;
    }

    bool Load2RobotSequence::HandleMovePickPositionXZ()
    {
        LogStep("HandleMovePickPositionXZ");

        m_parts->AxisX.Move(m_pickX);
        m_parts->AxisZ.Move(m_pickZ);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool Load2RobotSequence::HandlePreciserDown()
    {
        LogStep("HandlePreciserDown");

        m_parts->CylPreciser.down(false);

        return true;
    }

    bool Load2RobotSequence::HandleVacuumOn()
    {
        LogStep("HandleVacuumOn");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].vaccum(false);
        }

        return true;
    }

    bool Load2RobotSequence::HandleMoveSafeZAfterPick()
    {
      LogStep("HandleMoveSafeZAfterPick");

        m_parts->AxisZ.Move(m_safeZ);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool Load2RobotSequence::HandleMovePlacePositionXZ()
    {
        LogStep("HandleMovePlacePositionXZ");

        m_parts->AxisX.Move(m_placeX);
        m_parts->AxisZ.Move(m_placeZ);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool Load2RobotSequence::HandlePreciserUp()
    {
        LogStep("HandlePreciserUp");

        m_parts->CylPreciser.up(false);

        return true;
    }

    bool Load2RobotSequence::HandleVacuumOff()
    {
        LogStep("HandleVacuumOff");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].blow(false);
        }

        return true;
    }

    bool Load2RobotSequence::HandleMoveSafeZAfterPlace()
    {
        LogStep("HandleMoveSafeZAfterPlace");

        m_parts->AxisZ.Move(m_safeZ);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool Load2RobotSequence::HandleCheckRepeat()
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
            m_currentStep = PickPlaceStep::RailOpen;
            return false;
        }
        else if (m_currentIteration < m_repeatCount)
        {
            // 반복 계속
            m_currentStep = PickPlaceStep::RailOpen;
            return false;
        }

        // 반복 완료
        return true;
    }

    bool Load2RobotSequence::HandleComplete()
    {
        LogStep("HandleComplete");
        return false; // 완료
    }

    void Load2RobotSequence::MoveToNextStep()
    {
        int nextStep = static_cast<int>(m_currentStep) + 1;
        if (nextStep <= static_cast<int>(PickPlaceStep::Complete))
        {
            m_currentStep = static_cast<PickPlaceStep>(nextStep);
        }
    }

    bool Load2RobotSequence::IsStepTimeout() const
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_stepStartTime).count();
        return elapsed > m_moveTimeoutMs;
    }

    void Load2RobotSequence::LogStep(const std::string& message)
    {
        std::cout << "[Load2PickPlace] " << message << std::endl;
    }
}
