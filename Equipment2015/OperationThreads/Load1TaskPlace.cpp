#include "stdafx.h"
#include "Load1TaskPlace.h"
#include "../../EquipmentCore/include/Context.h"
#include <iostream>
#include <sstream>

namespace OperationThread
{
    Load1TaskPlace::Load1TaskPlace(std::shared_ptr<Load1Parts> parts)
        : TaskBase("Load1TaskPlace")
        , m_parts(parts)
        , m_placeX(300.0), m_placeY(150.0), m_placeZ(-12.0)
        , m_safeZ(0.0)
        , m_clampIndex(0)
        , m_vacuumIndex(0)
        , m_moveTimeoutMs(5000)
        , m_currentIteration(0)
        , m_repeatCount(2)
    {
    }

    Load1TaskPlace::~Load1TaskPlace()
    {
    }

    void Load1TaskPlace::OnInitialize(Context& ctx)
    {
        if (!m_parts)
        {
            ctx.SetLastError("Load1TaskPlace: Parts is null");
            EnterState(CS_ERROR);
            return;
        }

        m_currentIteration = 0;
        EnterState(RailOpen);
        LogStep(ctx, "Load1TaskPlace initialized");
    }

    TaskResult Load1TaskPlace::OnPoll(Context& ctx)
    {
        switch (GetStep())
        {
        case RailOpen:
            return HandleRailOpen(ctx);
        case MovePlacePositionXY:
            return HandleMovePlacePositionXY(ctx);
        case MovePlacePositionZ:
            return HandleMovePlacePositionZ(ctx);
        case ReleasePlace:
            return HandleReleasePlace(ctx);
        case BlowOn:
            return HandleBlowOn(ctx);
        case MoveSafeZAfterPlace:
            return HandleMoveSafeZAfterPlace(ctx);
        case CheckRepeat:
            return HandleCheckRepeat(ctx);
        case Complete:
            return HandleComplete(ctx);
        case CS_ERROR:
            return TR_ERROR;
        default:
            return SetErrorAndReturn(ctx, "Load1TaskPlace: Unknown state");
        }
    }

    // ============= 파라미터 설정 =============

    void Load1TaskPlace::SetPlacePosition(double x, double y, double z)
    {
        m_placeX = x;
        m_placeY = y;
        m_placeZ = z;
    }

    void Load1TaskPlace::SetSafeZ(double z)
    {
        m_safeZ = z;
    }

    void Load1TaskPlace::SetMoveTimeout(long timeoutMs)
    {
        m_moveTimeoutMs = timeoutMs;
    }

    void Load1TaskPlace::SetClampIndex(int index)
    {
        m_clampIndex = index;
    }

    void Load1TaskPlace::SetVacuumIndex(int index)
    {
        m_vacuumIndex = index;
    }

    // ============= 단계 처리 함수들 =============

    TaskResult Load1TaskPlace::HandleRailOpen(Context& ctx)
    {
        LogStep(ctx, "HandleRailOpen");

        if (!m_parts->CylBuffer.isBackward())
        {
            m_parts->CylBuffer.backward(false);
        }

        EnterState(MovePlacePositionXY);
        return TR_KEEP;
    }

    TaskResult Load1TaskPlace::HandleMovePlacePositionXY(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load1TaskPlace: MoveSafeZAfterPick timeout");
        }

        LogStep(ctx, "HandleMovePlacePositionXY");

        m_parts->AxisX.Move(m_placeX);
        m_parts->AxisY.Move(m_placeY);

        EnterStateWithTimeout(MovePlacePositionZ, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult Load1TaskPlace::HandleMovePlacePositionZ(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load1TaskPlace: MovePlacePositionXY timeout");
        }

        LogStep(ctx, "HandleMovePlacePositionZ");

        m_parts->AxisZ.Move(m_placeZ);

        EnterStateWithTimeout(ReleasePlace, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult Load1TaskPlace::HandleReleasePlace(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load1TaskPlace: MovePlacePositionZ timeout");
        }

        LogStep(ctx, "HandleReleasePlace");

        if (m_clampIndex >= 0 && m_clampIndex < static_cast<int>(m_parts->CylTransfer.size()))
        {
            m_parts->CylTransfer[m_clampIndex].release(false);
        }

        EnterState(BlowOn);
        return TR_KEEP;
    }

    TaskResult Load1TaskPlace::HandleBlowOn(Context& ctx)
    {
        LogStep(ctx, "HandleBlowOn");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].blow(false);
        }

        EnterState(MoveSafeZAfterPlace);
        return TR_KEEP;
    }

    TaskResult Load1TaskPlace::HandleMoveSafeZAfterPlace(Context& ctx)
    {
        LogStep(ctx, "HandleMoveSafeZAfterPlace");

        m_parts->AxisZ.Move(m_safeZ);

        EnterStateWithTimeout(CheckRepeat, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult Load1TaskPlace::HandleCheckRepeat(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load1TaskPlace: MoveSafeZAfterPlace timeout");
        }

        LogStep(ctx, "HandleCheckRepeat");

        ++m_currentIteration;

        std::ostringstream oss;
        oss << "Completed iteration: " << m_currentIteration;
        LogStep(ctx, oss.str());

        // 반복 횟수 확인
        if (m_repeatCount == 0 || m_currentIteration < m_repeatCount)
        {
            // 무한 반복 또는 반복 계속
            LogStep(ctx, "Repeating cycle...");
            EnterState(RailOpen);
            return TR_KEEP;
        }

        // 반복 완료
        EnterState(Complete);
        return TR_KEEP;
    }

    TaskResult Load1TaskPlace::HandleComplete(Context& ctx)
    {
        LogStep(ctx, "HandleComplete");
        return TR_NEXT;
    }
}
