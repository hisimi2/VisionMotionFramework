#include "stdafx.h"
#include "Load1TaskPick.h"
#include "../../EquipmentCore/include/Context.h"
#include <sstream>

namespace OperationThread
{
    Load1TaskPick::Load1TaskPick(std::shared_ptr<Load1Parts> parts)
        : TaskBase("Load1TaskPick")
        , m_parts(parts)
        , m_pickX(100.0), m_pickY(200.0), m_pickZ(-10.0)
        , m_safeZ(0.0)
        , m_clampIndex(0)
        , m_vacuumIndex(0)
        , m_moveTimeoutMs(5000)
        , m_currentIteration(0)
    {
    }

    Load1TaskPick::~Load1TaskPick()
    {
    }

    void Load1TaskPick::OnInitialize(Context& ctx)
    {
        if (!m_parts)
        {
            ctx.SetLastError("Load1TaskPick: Parts is null");
            EnterState(CS_ERROR);
            return;
        }

        m_currentIteration = 0;
        EnterState(RailOpen);
        LogStep(ctx, "Load1TaskPick initialized");
    }

    TaskResult Load1TaskPick::OnPoll(Context& ctx)
    {
        switch (GetStep())
        {
        case RailOpen:
            return HandleRailOpen(ctx);
        case MovePickPositionXY:
            return HandleMovePickPositionXY(ctx);
        case PreciserDown:
            return HandlePreciserDown(ctx);
        case MovePickPositionZ:
            return HandleMovePickPositionZ(ctx);
        case ClampPick:
            return HandleClampPick(ctx);
        case VacuumOn:
            return HandleVacuumOn(ctx);
        case MoveSafeZAfterPick:
            return HandleMoveSafeZAfterPick(ctx);
        case Complete:
            return HandleComplete(ctx);
        case CS_ERROR:
            return TR_ERROR;
        default:
            return SetErrorAndReturn(ctx, "Load1TaskPick: Unknown state");
        }
    }

    // ============= 파라미터 설정 =============

    void Load1TaskPick::SetPickPosition(double x, double y, double z)
    {
        m_pickX = x;
        m_pickY = y;
        m_pickZ = z;
    }

    void Load1TaskPick::SetSafeZ(double z)
    {
        m_safeZ = z;
    }

    void Load1TaskPick::SetMoveTimeout(long timeoutMs)
    {
        m_moveTimeoutMs = timeoutMs;
    }

    void Load1TaskPick::SetClampIndex(int index)
    {
        m_clampIndex = index;
    }

    void Load1TaskPick::SetVacuumIndex(int index)
    {
        m_vacuumIndex = index;
    }

    // ============= 단계 처리 함수들 =============

    TaskResult Load1TaskPick::HandleRailOpen(Context& ctx)
    {
        LogStep(ctx, "HandleRailOpen");

        if (!m_parts->CylBuffer.isBackward())
        {
            m_parts->CylBuffer.backward(false);
        }

        EnterState(MovePickPositionXY);
        return TR_KEEP;
    }

    TaskResult Load1TaskPick::HandleMovePickPositionXY(Context& ctx)
    {
        LogStep(ctx, "HandleMovePickPositionXY");

        m_parts->AxisX.Move(m_pickX);
        m_parts->AxisY.Move(m_pickY);

        EnterStateWithTimeout(PreciserDown, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult Load1TaskPick::HandlePreciserDown(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load1TaskPick: MovePickPositionXY timeout");
        }

        LogStep(ctx, "HandlePreciserDown");

        for (auto& cyl : m_parts->CylSetplate)
            cyl.down(false);

        EnterState(MovePickPositionZ);
        return TR_KEEP;
    }

    TaskResult Load1TaskPick::HandleMovePickPositionZ(Context& ctx)
    {
        LogStep(ctx, "HandleMovePickPositionZ");

        m_parts->AxisZ.Move(m_pickZ);

        EnterStateWithTimeout(ClampPick, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult Load1TaskPick::HandleClampPick(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load1TaskPick: MovePickPositionZ timeout");
        }

        LogStep(ctx, "HandleClampPick");

        if (m_clampIndex >= 0 && m_clampIndex < static_cast<int>(m_parts->CylTransfer.size()))
        {
            m_parts->CylTransfer[m_clampIndex].clamp(false);
        }

        EnterState(VacuumOn);
        return TR_KEEP;
    }

    TaskResult Load1TaskPick::HandleVacuumOn(Context& ctx)
    {
        LogStep(ctx, "HandleVacuumOn");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].vaccum(false);
        }

        EnterState(MoveSafeZAfterPick);
        return TR_KEEP;
    }

    TaskResult Load1TaskPick::HandleMoveSafeZAfterPick(Context& ctx)
    {
        LogStep(ctx, "HandleMoveSafeZAfterPick");

        m_parts->AxisZ.Move(m_safeZ);

        EnterStateWithTimeout(Complete, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult Load1TaskPick::HandleComplete(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load1TaskPick: MoveSafeZAfterPick timeout");
        }

        LogStep(ctx, "HandleComplete");

        ++m_currentIteration;

        std::ostringstream oss;
        oss << "Pick iteration " << m_currentIteration << " completed";
        std::string msg = oss.str();
        LogStep(ctx, msg);

        return TR_NEXT;
    }
}

