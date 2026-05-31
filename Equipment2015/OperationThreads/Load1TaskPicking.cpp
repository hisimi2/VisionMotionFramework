#include "stdafx.h"
#include "Load1TaskPicking.h"
#include <iostream>
#include <sstream>

namespace OperationThread
{
    Load1TaskPicking::Load1TaskPicking(LPVOID parts)
        : TaskBase("Load1TaskPicking")
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

    Load1TaskPicking::~Load1TaskPicking()
    {
    }

    void Load1TaskPicking::OnInitialize(Context& ctx)
    {
        if (!m_parts)
        {
            ctx.SetLastError("Load1TaskPicking: Parts is null");
            EnterState(CS_ERROR);
            return;
        }

        m_currentIteration = 0;
        EnterState(RailOpen);
        LogStep("Load1TaskPicking initialized");
    }

    TaskResult Load1TaskPicking::OnPoll(Context& ctx)
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
        case MovePlacePositionXY:   return HandleMovePlacePositionXY(ctx);
        case MovePlacePositionZ:    return HandleMovePlacePositionZ(ctx);
        case ReleasePlace:          return HandleReleasePlace(ctx);
        case BlowOn:                return HandleBlowOn(ctx);
        case MoveSafeZAfterPlace:   return HandleMoveSafeZAfterPlace(ctx);
        case CheckRepeat:           return HandleCheckRepeat(ctx);
        case Complete:              return HandleComplete(ctx);
        case CS_ERROR:              return TR_ERROR;
        default:                    return SetErrorAndReturn(ctx, "Load1TaskPicking: Unknown state");
        }
    }

    // ============= 파라미터 설정 =============

    void Load1TaskPicking::SetPickPosition(double x, double y, double z)
    {
        m_pickX = x;
        m_pickY = y;
        m_pickZ = z;
    }

    void Load1TaskPicking::SetPlacePosition(double x, double y, double z)
    {
        m_placeX = x;
        m_placeY = y;
        m_placeZ = z;
    }

    void Load1TaskPicking::SetSafeZ(double z)
    {
        m_safeZ = z;
    }

    void Load1TaskPicking::SetMoveTimeout(long timeoutMs)
    {
        m_moveTimeoutMs = timeoutMs;
    }

    void Load1TaskPicking::SetClampIndex(int index)
    {
        m_clampIndex = index;
    }

    void Load1TaskPicking::SetVacuumIndex(int index)
    {
        m_vacuumIndex = index;
    }

    // ============= 단계 처리 함수들 =============

    TaskResult Load1TaskPicking::HandleRailOpen(Context& ctx)
    {
        LogStep("HandleRailOpen");

        if (!m_parts->CylBuffer.isBackward())
        {
            m_parts->CylBuffer.backward(false);
        }

        EnterState(MovePickPositionXY);
        return TR_KEEP;
    }

    TaskResult Load1TaskPicking::HandleMovePickPositionXY(Context& ctx)
    {
        LogStep("HandleMovePickPositionXY");

        m_parts->AxisX.Move(m_pickX);
        m_parts->AxisY.Move(m_pickY);

        EnterStateWithTimeout(PreciserDown, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult Load1TaskPicking::HandlePreciserDown(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load1TaskPicking: MovePickPositionXY timeout");
        }

        LogStep("HandlePreciserDown");

        for (auto& cyl : m_parts->CylSetplate)
            cyl.down(false);

        EnterState(MovePickPositionZ);
        return TR_KEEP;
    }

    TaskResult Load1TaskPicking::HandleMovePickPositionZ(Context& ctx)
    {
        LogStep("HandleMovePickPositionZ");

        m_parts->AxisZ.Move(m_pickZ);

        EnterStateWithTimeout(ClampPick, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult Load1TaskPicking::HandleClampPick(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load1TaskPicking: MovePickPositionZ timeout");
        }

        LogStep("HandleClampPick");

        if (m_clampIndex >= 0 && m_clampIndex < static_cast<int>(m_parts->CylTransfer.size()))
        {
            m_parts->CylTransfer[m_clampIndex].clamp(false);
        }

        EnterState(VacuumOn);
        return TR_KEEP;
    }

    TaskResult Load1TaskPicking::HandleVacuumOn(Context& ctx)
    {
        LogStep("HandleVacuumOn");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].vaccum(false);
        }

        EnterState(MoveSafeZAfterPick);
        return TR_KEEP;
    }

    TaskResult Load1TaskPicking::HandleMoveSafeZAfterPick(Context& ctx)
    {
        LogStep("HandleMoveSafeZAfterPick");

        m_parts->AxisZ.Move(m_safeZ);

        EnterStateWithTimeout(MovePlacePositionXY, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult Load1TaskPicking::HandleMovePlacePositionXY(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load1TaskPicking: MoveSafeZAfterPick timeout");
        }

        LogStep("HandleMovePlacePositionXY");

        m_parts->AxisX.Move(m_placeX);
        m_parts->AxisY.Move(m_placeY);

        EnterStateWithTimeout(MovePlacePositionZ, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult Load1TaskPicking::HandleMovePlacePositionZ(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load1TaskPicking: MovePlacePositionXY timeout");
        }

        LogStep("HandleMovePlacePositionZ");

        m_parts->AxisZ.Move(m_placeZ);

        EnterStateWithTimeout(ReleasePlace, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult Load1TaskPicking::HandleReleasePlace(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load1TaskPicking: MovePlacePositionZ timeout");
        }

        LogStep("HandleReleasePlace");

        if (m_clampIndex >= 0 && m_clampIndex < static_cast<int>(m_parts->CylTransfer.size()))
        {
            m_parts->CylTransfer[m_clampIndex].release(false);
        }

        EnterState(BlowOn);
        return TR_KEEP;
    }

    TaskResult Load1TaskPicking::HandleBlowOn(Context& ctx)
    {
        LogStep("HandleBlowOn");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].blow(false);
        }

        EnterState(MoveSafeZAfterPlace);
        return TR_KEEP;
    }

    TaskResult Load1TaskPicking::HandleMoveSafeZAfterPlace(Context& ctx)
    {
        LogStep("HandleMoveSafeZAfterPlace");

        m_parts->AxisZ.Move(m_safeZ);

        EnterStateWithTimeout(CheckRepeat, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult Load1TaskPicking::HandleCheckRepeat(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load1TaskPicking: MoveSafeZAfterPlace timeout");
        }

        LogStep("HandleCheckRepeat");

        ++m_currentIteration;

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

    TaskResult Load1TaskPicking::HandleComplete(Context& ctx)
    {
        LogStep("HandleComplete");
        return TR_NEXT;
    }

    void Load1TaskPicking::LogStep(const std::string& message)
    {
        std::cout << "[Load1PickPlace] " << message << std::endl;
    }
}
