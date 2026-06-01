#include "stdafx.h"
#include "Load1TaskPick.h"
#include <iostream>
#include <sstream>

namespace OperationThread
{
    Load1TaskPick::Load1TaskPick(LPVOID parts)
        : TaskBase("Load1TaskPick")
        , m_parts((Load1Parts*)parts)
        , m_pickX(100.0), m_pickY(200.0), m_pickZ(-10.0)
        , m_placeX(300.0), m_placeY(150.0), m_placeZ(-12.0)
        , m_safeZ(0.0)
        , m_clampIndex(0)
        , m_vacuumIndex(0)
        , m_moveTimeoutMs(5000)
        , m_currentIteration(0)
        , m_repeatCount(2)
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
        LogStep("Load1TaskPick initialized");
    }

    TaskResult Load1TaskPick::OnPoll(Context& ctx)
    {
        switch (GetState())
        {
        case RailOpen:              return HandleRailOpen(ctx);
        case MovePickPositionXY:    return HandleMovePickPositionXY(ctx);
        case PreciserDown:          return HandlePreciserDown(ctx);
        case MovePickPositionZ:     return HandleMovePickPositionZ(ctx);
        case ClampPick:             return HandleClampPick(ctx);
        case VacuumOn:              return HandleVacuumOn(ctx);
        case MoveSafeZAfterPick:    return HandleMoveSafeZAfterPick(ctx);
        case CS_ERROR:              return TR_ERROR;
        default:                    return SetErrorAndReturn(ctx, "Load1TaskPick: Unknown state");
        }
    }

    // ============= 파라미터 설정 =============

    void Load1TaskPick::SetPickPosition(double x, double y, double z)
    {
        m_pickX = x;
        m_pickY = y;
        m_pickZ = z;
    }

    void Load1TaskPick::SetPlacePosition(double x, double y, double z)
    {
        m_placeX = x;
        m_placeY = y;
        m_placeZ = z;
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
        LogStep("HandleRailOpen");

        if (!m_parts->CylBuffer.isBackward())
        {
            m_parts->CylBuffer.backward(false);
        }

        EnterState(MovePickPositionXY);
        return TR_KEEP;
    }

    TaskResult Load1TaskPick::HandleMovePickPositionXY(Context& ctx)
    {
        LogStep("HandleMovePickPositionXY");

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

        LogStep("HandlePreciserDown");

        for (auto& cyl : m_parts->CylSetplate)
            cyl.down(false);

        EnterState(MovePickPositionZ);
        return TR_KEEP;
    }

    TaskResult Load1TaskPick::HandleMovePickPositionZ(Context& ctx)
    {
        LogStep("HandleMovePickPositionZ");

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

        LogStep("HandleClampPick");

        if (m_clampIndex >= 0 && m_clampIndex < static_cast<int>(m_parts->CylTransfer.size()))
        {
            m_parts->CylTransfer[m_clampIndex].clamp(false);
        }

        EnterState(VacuumOn);
        return TR_KEEP;
    }

    TaskResult Load1TaskPick::HandleVacuumOn(Context& ctx)
    {
        LogStep("HandleVacuumOn");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].vaccum(false);
        }

        EnterState(MoveSafeZAfterPick);
        return TR_KEEP;
    }

    TaskResult Load1TaskPick::HandleMoveSafeZAfterPick(Context& ctx)
    {
        LogStep("HandleMoveSafeZAfterPick");

        m_parts->AxisZ.Move(m_safeZ);

        EnterStateWithTimeout(Complete, m_moveTimeoutMs);
        return TR_NEXT;
    }

    void Load1TaskPick::LogStep(const std::string& message)
    {
        std::cout << "[Load1PickPlace] " << message << std::endl;
    }
}
