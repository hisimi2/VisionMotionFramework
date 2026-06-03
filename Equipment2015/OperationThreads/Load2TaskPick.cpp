#include "stdafx.h"
#include "Load2TaskPick.h"
#include "../../EquipmentCore/include/Context.h"
#include <iostream>
#include <sstream>

namespace OperationThread
{
    Load2TaskPick::Load2TaskPick(std::shared_ptr<Load2Parts> parts)
        : TaskBase("Load2TaskPick")
        , m_parts(parts)
        , m_pickX(100.0), m_pickZ(-10.0)
        , m_xPitchWidth(50.0)
        , m_safeZ(0.0)
        , m_vacuumIndex(0)
        , m_moveTimeoutMs(5000)
    {
    }

    Load2TaskPick::~Load2TaskPick()
    {
    }

    void Load2TaskPick::OnInitialize(Context& ctx)
    {
        if (!m_parts)
        {
            ctx.SetLastError("Load2TaskPick: Parts is null");
            EnterState(CS_ERROR);
            return;
        }

        EnterState(RailOpen);
        LogStep(ctx, "Load2TaskPick initialized");
    }

    TaskResult Load2TaskPick::OnPoll(Context& ctx)
    {
        switch (GetStep())
        {
        case RailOpen:
            return HandleRailOpen(ctx);
        case MovePickPositionX:
            return HandleMovePickPositionX(ctx);
        case PreciserDown:
            return HandlePreciserDown(ctx);
        case MovePickPositionZ:
            return HandleMovePickPositionZ(ctx);
        case VacuumOn:
            return HandleVacuumOn(ctx);
        case MoveSafeZAfterPick:
            return HandleMoveSafeZAfterPick(ctx);
        case Complete:
            return HandleComplete(ctx);
        case CS_ERROR:
            return TR_ERROR;
        default:
            return SetErrorAndReturn(ctx, "Load2TaskPick: Unknown state");
        }
    }

    // ============= 파라미터 설정 =============

    void Load2TaskPick::SetPickPosition(double x, double z)
    {
        m_pickX = x;
        m_pickZ = z;
    }

    void Load2TaskPick::SetXPitchWidth(double width)
    {
        m_xPitchWidth = width;
    }

    void Load2TaskPick::SetSafeZ(double z)
    {
        m_safeZ = z;
    }

    void Load2TaskPick::SetMoveTimeout(long timeoutMs)
    {
        m_moveTimeoutMs = timeoutMs;
    }

    void Load2TaskPick::SetVacuumIndex(int index)
    {
        m_vacuumIndex = index;
    }

    // ============= 단계 처리 함수들 =============

    TaskResult Load2TaskPick::HandleRailOpen(Context& ctx)
    {
        LogStep(ctx, "HandleRailOpen");

        if (!m_parts->CylRail.isOpen())
        {
            m_parts->CylRail.open(false);
        }

        EnterState(MovePickPositionX);
        return TR_KEEP;
    }

    TaskResult Load2TaskPick::HandleMovePickPositionX(Context& ctx)
    {
        LogStep(ctx, "HandleMovePickPositionX");

        m_parts->AxisX.Move(m_pickX);
        m_parts->CylXPitch.narrow(false);

        EnterStateWithTimeout(PreciserDown, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult Load2TaskPick::HandlePreciserDown(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load2TaskPick: MovePickPositionX timeout");
        }

        LogStep(ctx, "HandlePreciserDown");

        m_parts->CylPreciser.down(false);

        EnterState(MovePickPositionZ);
        return TR_KEEP;
    }

    TaskResult Load2TaskPick::HandleMovePickPositionZ(Context& ctx)
    {
        LogStep(ctx, "HandleMovePickPositionZ");

        m_parts->AxisZ.Move(m_pickZ);

        EnterStateWithTimeout(VacuumOn, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult Load2TaskPick::HandleVacuumOn(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load2TaskPick: MovePickPositionZ timeout");
        }

        LogStep(ctx, "HandleVacuumOn");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].vaccum(false);
        }

        EnterState(MoveSafeZAfterPick);
        return TR_KEEP;
    }

    TaskResult Load2TaskPick::HandleMoveSafeZAfterPick(Context& ctx)
    {
        LogStep(ctx, "HandleMoveSafeZAfterPick");

        m_parts->AxisZ.Move(m_safeZ);

        EnterStateWithTimeout(Complete, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult Load2TaskPick::HandleComplete(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load2TaskPick: MoveSafeZAfterPick timeout");
        }

        LogStep(ctx, "HandleComplete");
        return TR_NEXT;
    }

}

