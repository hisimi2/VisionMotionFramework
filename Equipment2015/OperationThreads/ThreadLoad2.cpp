#include "stdafx.h"
#include "ThreadLoad2.h"
#include <iostream>

namespace OperationThread
{
    ThreadLoad2::ThreadLoad2(LPVOID parts)
        : TaskBase("ThreadLoad2")
        , m_parts((Load2Parts*)parts)
        , m_currentIteration(0)
        , m_successCount(0)
        , m_moveTimeoutMs(3000)
        , m_pickX(100.0), m_pickZ(-10.0)
        , m_placeX(300.0), m_placeZ(-12.0)
        , m_safeZ(0.0)
        , m_vacuumIndex(0)
        , m_repeatCount(1)
    {
    }

    ThreadLoad2::~ThreadLoad2()
    {
    }

    void ThreadLoad2::OnInitialize(Context& ctx)
    {
        if (!m_parts)
        {
            ctx.SetLastError("ThreadLoad2: Parts is null");
            EnterState(CS_ERROR);
            return;
        }

        m_currentIteration = 0;
        m_successCount = 0;
        EnterState(RailOpen);
        LogStep("ThreadLoad2 initialized");
    }

    EC::TaskResult ThreadLoad2::OnPoll(Context& ctx)
    {
        switch (GetState())
        {
        case RailOpen:              return HandleRailOpen(ctx);
        case MovePickPositionXZ:    return HandleMovePickPositionXZ(ctx);
        case PreciserDown:          return HandlePreciserDown(ctx);
        case VacuumOn:              return HandleVacuumOn(ctx);
        case MoveSafeZAfterPick:    return HandleMoveSafeZAfterPick(ctx);
        case MovePlacePositionXZ:   return HandleMovePlacePositionXZ(ctx);
        case PreciserUp:            return HandlePreciserUp(ctx);
        case VacuumOff:             return HandleVacuumOff(ctx);
        case MoveSafeZAfterPlace:   return HandleMoveSafeZAfterPlace(ctx);
        case CheckRepeat:           return HandleCheckRepeat(ctx);
        case Complete:              return HandleComplete(ctx);
        case CS_ERROR:              return TR_ERROR;
        default:                    return SetErrorAndReturn(ctx, "ThreadLoad2: Unknown state");
        }
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

    // ============= 단계 처리 함수들 =============

    EC::TaskResult ThreadLoad2::HandleRailOpen(Context& ctx)
    {
        LogStep("HandleRailOpen");

        if (!m_parts->CylRail.isOpen())
        {
            m_parts->CylRail.open(false);
        }

        EnterState(MovePickPositionXZ);
        return TR_KEEP;
    }

    EC::TaskResult ThreadLoad2::HandleMovePickPositionXZ(Context& ctx)
    {
        LogStep("HandleMovePickPositionXZ");

        m_parts->AxisX.Move(m_pickX);
        m_parts->AxisZ.Move(m_pickZ);

        EnterStateWithTimeout(PreciserDown, m_moveTimeoutMs);
        return TR_KEEP;
    }

    EC::TaskResult ThreadLoad2::HandlePreciserDown(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "ThreadLoad2: MovePickPositionXZ timeout");
        }

        LogStep("HandlePreciserDown");

        m_parts->CylPreciser.down(false);

        EnterState(VacuumOn);
        return TR_KEEP;
    }

    EC::TaskResult ThreadLoad2::HandleVacuumOn(Context& ctx)
    {
        LogStep("HandleVacuumOn");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].vaccum(false);
        }

        EnterState(MoveSafeZAfterPick);
        return TR_KEEP;
    }

    EC::TaskResult ThreadLoad2::HandleMoveSafeZAfterPick(Context& ctx)
    {
        LogStep("HandleMoveSafeZAfterPick");

        m_parts->AxisZ.Move(m_safeZ);

        EnterStateWithTimeout(MovePlacePositionXZ, m_moveTimeoutMs);
        return TR_KEEP;
    }

    EC::TaskResult ThreadLoad2::HandleMovePlacePositionXZ(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "ThreadLoad2: MoveSafeZAfterPick timeout");
        }

        LogStep("HandleMovePlacePositionXZ");

        m_parts->AxisX.Move(m_placeX);
        m_parts->AxisZ.Move(m_placeZ);

        EnterStateWithTimeout(PreciserUp, m_moveTimeoutMs);
        return TR_KEEP;
    }

    EC::TaskResult ThreadLoad2::HandlePreciserUp(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "ThreadLoad2: MovePlacePositionXZ timeout");
        }

        LogStep("HandlePreciserUp");

        m_parts->CylPreciser.up(false);

        EnterState(VacuumOff);
        return TR_KEEP;
    }

    EC::TaskResult ThreadLoad2::HandleVacuumOff(Context& ctx)
    {
        LogStep("HandleVacuumOff");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].blow(false);
        }

        EnterState(MoveSafeZAfterPlace);
        return TR_KEEP;
    }

    EC::TaskResult ThreadLoad2::HandleMoveSafeZAfterPlace(Context& ctx)
    {
        LogStep("HandleMoveSafeZAfterPlace");

        m_parts->AxisZ.Move(m_safeZ);

        EnterStateWithTimeout(CheckRepeat, m_moveTimeoutMs);
        return TR_KEEP;
    }

    EC::TaskResult ThreadLoad2::HandleCheckRepeat(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "ThreadLoad2: MoveSafeZAfterPlace timeout");
        }

        LogStep("HandleCheckRepeat");

        ++m_currentIteration;
        ++m_successCount;

        std::ostringstream oss;
        oss << "Completed iteration: " << m_currentIteration;
        LogStep(oss.str());

        // 반복 횟수 확인
        if (m_repeatCount == 0 || m_currentIteration < m_repeatCount)
        {
            // 무한 반복 또는 반복 계속
            LogStep("Repeating cycle...");
            EnterState(RailOpen);
            return TR_KEEP;
        }

        // 반복 완료
        EnterState(Complete);
        return TR_KEEP;
    }

    EC::TaskResult ThreadLoad2::HandleComplete(Context& ctx)
    {
        LogStep("HandleComplete");
        return TR_NEXT;
    }

    void ThreadLoad2::LogStep(const std::string& message)
    {
        std::cout << "[Load2PickPlace] " << message << std::endl;
    }
}
