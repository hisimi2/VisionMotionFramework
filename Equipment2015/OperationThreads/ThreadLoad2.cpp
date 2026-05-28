#include "stdafx.h"
#include "ThreadLoad2.h"
#include <iostream>
#include <sstream>
#include "Actuators\Load2Parts.h"

namespace OperationThread
{
    ThreadLoad2::ThreadLoad2(LPVOID parts, int repeatCount)
        : m_parts((Load2Parts*)parts)
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

    ThreadLoad2::~ThreadLoad2()
    {
    }

    void ThreadLoad2::OnInitialize()
    {
        if (!m_parts)
        throw std::runtime_error("ThreadLoad2: Parts is null");

        m_initialized = true;
        m_currentIteration = 0;
        m_successCount = 0;
        m_currentStep = PickPlaceStep::RailOpen;

        LogStep("ThreadLoad2 initialized");
    }

    bool ThreadLoad2::OnPoll()
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

    void ThreadLoad2::OnCleanup()
    {
        LogStep("ThreadLoad2 cleanup completed");
    }

    void ThreadLoad2::OnError(const std::string& errorMsg)
    {
        m_lastError = errorMsg;
        std::cerr << "ThreadLoad2 Error: " << errorMsg << std::endl;
    }

    void ThreadLoad2::SetPickPosition(double x, double z)
    {
        m_pickX = x;
        m_pickZ = z;
    }

    void ThreadLoad2::SetPlacePosition(double x, double z)
    {
        m_placeX = x;
        m_placeZ = z;
    }

    void ThreadLoad2::SetSafeZ(double z)
    {
        m_safeZ = z;
    }

    void ThreadLoad2::SetMoveTimeout(long timeoutMs)
    {
        m_moveTimeoutMs = timeoutMs;
    }

    void ThreadLoad2::SetVacuumIndex(int index)
    {
        m_vacuumIndex = index;
    }

    ThreadLoad2::PickPlaceStep ThreadLoad2::GetCurrentStep() const
    {
        return m_currentStep;
    }

    int ThreadLoad2::GetCurrentIteration() const
    {
        return m_currentIteration;
    }

    int ThreadLoad2::GetSuccessCount() const
    {
        return m_successCount;
    }

    // ============= 단계 처리 함수들 =============
    bool ThreadLoad2::HandleRailOpen()
    {
        LogStep("HandleRailOpen");

        if (!m_parts->CylRail.isOpen())
        {
            m_parts->CylRail.open(false);
        }

        return true;
    }

    bool ThreadLoad2::HandleMovePickPositionXZ()
    {
        LogStep("HandleMovePickPositionXZ");

        m_parts->AxisX.Move(m_pickX);
        m_parts->AxisZ.Move(m_pickZ);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool ThreadLoad2::HandlePreciserDown()
    {
        LogStep("HandlePreciserDown");

        m_parts->CylPreciser.down(false);

        return true;
    }

    bool ThreadLoad2::HandleVacuumOn()
    {
        LogStep("HandleVacuumOn");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].vaccum(false);
        }

        return true;
    }

    bool ThreadLoad2::HandleMoveSafeZAfterPick()
    {
      LogStep("HandleMoveSafeZAfterPick");

        m_parts->AxisZ.Move(m_safeZ);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool ThreadLoad2::HandleMovePlacePositionXZ()
    {
        LogStep("HandleMovePlacePositionXZ");

        m_parts->AxisX.Move(m_placeX);
        m_parts->AxisZ.Move(m_placeZ);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool ThreadLoad2::HandlePreciserUp()
    {
        LogStep("HandlePreciserUp");

        m_parts->CylPreciser.up(false);

        return true;
    }

    bool ThreadLoad2::HandleVacuumOff()
    {
        LogStep("HandleVacuumOff");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].blow(false);
        }

        return true;
    }

    bool ThreadLoad2::HandleMoveSafeZAfterPlace()
    {
        LogStep("HandleMoveSafeZAfterPlace");

        m_parts->AxisZ.Move(m_safeZ);

        m_stepStartTime = std::chrono::steady_clock::now();
        return false; // 타임아웃 대기
    }

    bool ThreadLoad2::HandleCheckRepeat()
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

    bool ThreadLoad2::HandleComplete()
    {
        LogStep("HandleComplete");
        return false; // 완료
    }

    void ThreadLoad2::MoveToNextStep()
    {
        int nextStep = static_cast<int>(m_currentStep) + 1;
        if (nextStep <= static_cast<int>(PickPlaceStep::Complete))
        {
            m_currentStep = static_cast<PickPlaceStep>(nextStep);
        }
    }

    bool ThreadLoad2::IsStepTimeout() const
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_stepStartTime).count();
        return elapsed > m_moveTimeoutMs;
    }

    void ThreadLoad2::LogStep(const std::string& message)
    {
        std::cout << "[Load2PickPlace] " << message << std::endl;
    }
}
