#include "stdafx.h"
#include "Load2PickPlaceSequence.h"
#include <iostream>
#include <sstream>
#include "Actuators\Load2Parts.h"

namespace OperationThread
{
    Load2PickPlaceSequence::Load2PickPlaceSequence(Load2Parts* parts, int repeatCount)
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

    Load2PickPlaceSequence::~Load2PickPlaceSequence()
    {
    }

    void Load2PickPlaceSequence::OnInitialize()
    {
        if (!m_parts)
        throw std::runtime_error("Load2PickPlaceSequence: Parts is null");

        m_initialized = true;
        m_currentIteration = 0;
        m_successCount = 0;
        m_currentStep = PickPlaceStep::RailOpen;

        LogStep("Load2PickPlaceSequence initialized");
    }

    bool Load2PickPlaceSequence::OnPoll()
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

    void Load2PickPlaceSequence::OnCleanup()
    {
        LogStep("Load2PickPlaceSequence cleanup completed");
    }

    void Load2PickPlaceSequence::OnError(const std::string& errorMsg)
    {
        m_lastError = errorMsg;
        std::cerr << "Load2PickPlaceSequence Error: " << errorMsg << std::endl;
    }

    void Load2PickPlaceSequence::SetPickPosition(double x, double z)
    {
        m_pickX = x;
        m_pickZ = z;
    }

    void Load2PickPlaceSequence::SetPlacePosition(double x, double z)
    {
        m_placeX = x;
        m_placeZ = z;
    }

    void Load2PickPlaceSequence::SetSafeZ(double z)
    {
        m_safeZ = z;
    }

    void Load2PickPlaceSequence::SetMoveTimeout(long timeoutMs)
    {
        m_moveTimeoutMs = timeoutMs;
    }

    void Load2PickPlaceSequence::SetVacuumIndex(int index)
    {
        m_vacuumIndex = index;
    }

    Load2PickPlaceSequence::PickPlaceStep Load2PickPlaceSequence::GetCurrentStep() const
    {
        return m_currentStep;
    }

    int Load2PickPlaceSequence::GetCurrentIteration() const
    {
        return m_currentIteration;
    }

    int Load2PickPlaceSequence::GetSuccessCount() const
    {
        return m_successCount;
    }

    // ============= 단계 처리 함수들 =============

    bool Load2PickPlaceSequence::HandleRailOpen()
    {
        LogStep("HandleRailOpen");

        if (!m_parts->CylRail.isOpen())
        {
            m_parts->CylRail.open(false);
        }

        return true;
    }

    bool Load2PickPlaceSequence::HandleMovePickPositionXZ()
    {
        LogStep("HandleMovePickPositionXZ");

        m_parts->AxisX.Move(m_pickX);
        m_parts->AxisZ.Move(m_pickZ);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool Load2PickPlaceSequence::HandlePreciserDown()
    {
        LogStep("HandlePreciserDown");

        m_parts->CylPreciser.down(false);

        return true;
    }

    bool Load2PickPlaceSequence::HandleVacuumOn()
    {
        LogStep("HandleVacuumOn");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].vaccum(false);
        }

        return true;
    }

    bool Load2PickPlaceSequence::HandleMoveSafeZAfterPick()
    {
      LogStep("HandleMoveSafeZAfterPick");

        m_parts->AxisZ.Move(m_safeZ);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool Load2PickPlaceSequence::HandleMovePlacePositionXZ()
    {
        LogStep("HandleMovePlacePositionXZ");

        m_parts->AxisX.Move(m_placeX);
        m_parts->AxisZ.Move(m_placeZ);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool Load2PickPlaceSequence::HandlePreciserUp()
    {
        LogStep("HandlePreciserUp");

        m_parts->CylPreciser.up(false);

        return true;
    }

    bool Load2PickPlaceSequence::HandleVacuumOff()
    {
        LogStep("HandleVacuumOff");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].blow(false);
        }

        return true;
    }

    bool Load2PickPlaceSequence::HandleMoveSafeZAfterPlace()
    {
        LogStep("HandleMoveSafeZAfterPlace");

        m_parts->AxisZ.Move(m_safeZ);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool Load2PickPlaceSequence::HandleCheckRepeat()
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

    bool Load2PickPlaceSequence::HandleComplete()
    {
        LogStep("HandleComplete");
        return false; // 완료
    }

    void Load2PickPlaceSequence::MoveToNextStep()
    {
        int nextStep = static_cast<int>(m_currentStep) + 1;
        if (nextStep <= static_cast<int>(PickPlaceStep::Complete))
        {
            m_currentStep = static_cast<PickPlaceStep>(nextStep);
        }
    }

    bool Load2PickPlaceSequence::IsStepTimeout() const
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_stepStartTime).count();
        return elapsed > m_moveTimeoutMs;
    }

    void Load2PickPlaceSequence::LogStep(const std::string& message)
    {
        std::cout << "[Load2PickPlace] " << message << std::endl;
    }
}
