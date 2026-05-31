#include "stdafx.h"
#include "ThreadLoad1.h"
#include <iostream>
#include <sstream>

namespace OperationThread
{
    using namespace EC;

    ThreadLoad1::ThreadLoad1(LPVOID parts, int repeatCount)
        : TaskBase("ThreadLoad1")
        , m_parts((Load1Parts*)parts)
        , m_pickX(100.0), m_pickY(200.0), m_pickZ(-10.0)
        , m_placeX(300.0), m_placeY(150.0), m_placeZ(-12.0)
        , m_safeZ(0.0)
        , m_clampIndex(0)
        , m_vacuumIndex(0)
        , m_moveTimeoutMs(5000)
        , m_repeatCount(repeatCount)
        , m_currentIteration(0)
    {
    }

    ThreadLoad1::~ThreadLoad1()
    {
    }

    void ThreadLoad1::OnInitialize(Context& ctx)
    {
        if (!m_parts)
        {
            ctx.SetLastError("ThreadLoad1: Parts is null");
            EnterState(CS_ERROR);
            return;
        }

        m_currentIteration = 0;
        EnterState(RailOpen);
        LogStep("ThreadLoad1 initialized");
    }

    TaskResult ThreadLoad1::OnPoll(Context& ctx)
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
        default:                    return SetErrorAndReturn(ctx, "ThreadLoad1: Unknown state");
        }
    }

    // ============= 파라미터 설정 =============

    void ThreadLoad1::SetPickPosition(double x, double y, double z)
    {
        m_pickX = x;
        m_pickY = y;
        m_pickZ = z;
    }

    void ThreadLoad1::SetPlacePosition(double x, double y, double z)
    {
        m_placeX = x;
        m_placeY = y;
        m_placeZ = z;
    }

    void ThreadLoad1::SetSafeZ(double z)
    {
        m_safeZ = z;
    }

    void ThreadLoad1::SetMoveTimeout(long timeoutMs)
    {
        m_moveTimeoutMs = timeoutMs;
    }

    void ThreadLoad1::SetClampIndex(int index)
    {
        m_clampIndex = index;
    }

    void ThreadLoad1::SetVacuumIndex(int index)
    {
        m_vacuumIndex = index;
    }

    // ============= 단계 처리 함수들 =============

    TaskResult ThreadLoad1::HandleRailOpen(Context& ctx)
    {
        LogStep("HandleRailOpen");

        if (!m_parts->CylBuffer.isBackward())
        {
            m_parts->CylBuffer.backward(false);
        }

        EnterState(MovePickPositionXY);
        return TR_KEEP;
    }

    TaskResult ThreadLoad1::HandleMovePickPositionXY(Context& ctx)
    {
        LogStep("HandleMovePickPositionXY");

        m_parts->AxisX.Move(m_pickX);
        m_parts->AxisY.Move(m_pickY);

        EnterStateWithTimeout(PreciserDown, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult ThreadLoad1::HandlePreciserDown(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "ThreadLoad1: MovePickPositionXY timeout");
        }

        LogStep("HandlePreciserDown");

        for (auto& cyl : m_parts->CylSetplate)
            cyl.down(false);

        EnterState(MovePickPositionZ);
        return TR_KEEP;
    }

    TaskResult ThreadLoad1::HandleMovePickPositionZ(Context& ctx)
    {
        LogStep("HandleMovePickPositionZ");

        m_parts->AxisZ.Move(m_pickZ);

        EnterStateWithTimeout(ClampPick, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult ThreadLoad1::HandleClampPick(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "ThreadLoad1: MovePickPositionZ timeout");
        }

        LogStep("HandleClampPick");

        if (m_clampIndex >= 0 && m_clampIndex < static_cast<int>(m_parts->CylTransfer.size()))
        {
            m_parts->CylTransfer[m_clampIndex].clamp(false);
        }

        EnterState(VacuumOn);
        return TR_KEEP;
    }

    TaskResult ThreadLoad1::HandleVacuumOn(Context& ctx)
    {
        LogStep("HandleVacuumOn");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].vaccum(false);
        }

        EnterState(MoveSafeZAfterPick);
        return TR_KEEP;
    }

    TaskResult ThreadLoad1::HandleMoveSafeZAfterPick(Context& ctx)
    {
        LogStep("HandleMoveSafeZAfterPick");

        m_parts->AxisZ.Move(m_safeZ);

        EnterStateWithTimeout(MovePlacePositionXY, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult ThreadLoad1::HandleMovePlacePositionXY(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "ThreadLoad1: MoveSafeZAfterPick timeout");
        }

        LogStep("HandleMovePlacePositionXY");

        m_parts->AxisX.Move(m_placeX);
        m_parts->AxisY.Move(m_placeY);

        EnterStateWithTimeout(MovePlacePositionZ, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult ThreadLoad1::HandleMovePlacePositionZ(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "ThreadLoad1: MovePlacePositionXY timeout");
        }

        LogStep("HandleMovePlacePositionZ");

        m_parts->AxisZ.Move(m_placeZ);

        EnterStateWithTimeout(ReleasePlace, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult ThreadLoad1::HandleReleasePlace(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "ThreadLoad1: MovePlacePositionZ timeout");
        }

        LogStep("HandleReleasePlace");

        if (m_clampIndex >= 0 && m_clampIndex < static_cast<int>(m_parts->CylTransfer.size()))
        {
            m_parts->CylTransfer[m_clampIndex].release(false);
        }

        EnterState(BlowOn);
        return TR_KEEP;
    }

    TaskResult ThreadLoad1::HandleBlowOn(Context& ctx)
    {
        LogStep("HandleBlowOn");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].blow(false);
        }

        EnterState(MoveSafeZAfterPlace);
        return TR_KEEP;
    }

    TaskResult ThreadLoad1::HandleMoveSafeZAfterPlace(Context& ctx)
    {
        LogStep("HandleMoveSafeZAfterPlace");

        m_parts->AxisZ.Move(m_safeZ);

        EnterStateWithTimeout(CheckRepeat, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult ThreadLoad1::HandleCheckRepeat(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "ThreadLoad1: MoveSafeZAfterPlace timeout");
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

    TaskResult ThreadLoad1::HandleComplete(Context& ctx)
    {
        LogStep("HandleComplete");
        return TR_NEXT;
    }

    void ThreadLoad1::LogStep(const std::string& message)
    {
        std::cout << "[Load1PickPlace] " << message << std::endl;
    }
}
