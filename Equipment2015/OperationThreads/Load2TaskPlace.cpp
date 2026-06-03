#include "stdafx.h"
#include "Load2TaskPlace.h"
#include "../../EquipmentCore/include/Context.h"
#include <iostream>
#include <sstream>

namespace OperationThread
{
    Load2TaskPlace::Load2TaskPlace(std::shared_ptr<Load2Parts> parts)
        : TaskBase("Load2TaskPlace")
        , m_parts(parts)
        , m_placeX(200.0), m_placeZ(-12.0)
        , m_safeZ(0.0)
        , m_vacuumIndex(0)
        , m_moveTimeoutMs(5000)
    {
    }

    Load2TaskPlace::~Load2TaskPlace()
    {
    }

    void Load2TaskPlace::OnInitialize(Context& ctx)
    {
        if (!m_parts)
        {
            ctx.SetLastError("Load2TaskPlace: Parts is null");
            EnterState(CS_ERROR);
            return;
        }

        EnterState(MovePlacePositionX);
        LogStep(ctx, "Load2TaskPlace initialized");
    }

    TaskResult Load2TaskPlace::OnPoll(Context& ctx)
    {
        switch (GetStep())
        {
        case MovePlacePositionX:
            return HandleMovePlacePositionX(ctx);
        case PusherForward:
            return HandlePusherForward(ctx);
        case MovePlacePositionZ:
            return HandleMovePlacePositionZ(ctx);
        case BlowOn:
            return HandleBlowOn(ctx);
        case PusherBackward:
            return HandlePusherBackward(ctx);
        case MoveSafeZAfterPlace:
            return HandleMoveSafeZAfterPlace(ctx);
        case CS_ERROR:
            return TR_ERROR;
        default:
            return SetErrorAndReturn(ctx, "Load2TaskPlace: Unknown state");
        }
    }

    // ============= 파라미터 설정 =============

    void Load2TaskPlace::SetPlacePosition(double x, double z)
    {
        m_placeX = x;
        m_placeZ = z;
    }

    void Load2TaskPlace::SetSafeZ(double z)
    {
        m_safeZ = z;
    }

    void Load2TaskPlace::SetMoveTimeout(long timeoutMs)
    {
        m_moveTimeoutMs = timeoutMs;
    }

    void Load2TaskPlace::SetVacuumIndex(int index)
    {
        m_vacuumIndex = index;
    }

    // ============= 단계 처리 함수들 =============

    TaskResult Load2TaskPlace::HandleMovePlacePositionX(Context& ctx)
    {
        LogStep(ctx, "HandleMovePlacePositionX");

        m_parts->AxisX.Move(m_placeX);

        EnterStateWithTimeout(PusherForward, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult Load2TaskPlace::HandlePusherForward(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load2TaskPlace: MovePlacePositionX timeout");
        }

        LogStep(ctx, "HandlePusherForward");

        m_parts->CylPusher.forward(false);

        EnterState(MovePlacePositionZ);
        return TR_KEEP;
    }

    TaskResult Load2TaskPlace::HandleMovePlacePositionZ(Context& ctx)
    {
        LogStep(ctx, "HandleMovePlacePositionZ");

        m_parts->AxisZ.Move(m_placeZ);

        EnterStateWithTimeout(BlowOn, m_moveTimeoutMs);
        return TR_KEEP;
    }

    TaskResult Load2TaskPlace::HandleBlowOn(Context& ctx)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "Load2TaskPlace: MovePlacePositionZ timeout");
        }

        LogStep(ctx, "HandleBlowOn");

        if (m_vacuumIndex >= 0 && m_vacuumIndex < static_cast<int>(m_parts->PickVacuum.size()))
        {
            m_parts->PickVacuum[m_vacuumIndex].blow(false);
        }

        EnterState(PusherBackward);
        return TR_KEEP;
    }

    TaskResult Load2TaskPlace::HandlePusherBackward(Context& ctx)
    {
        LogStep(ctx, "HandlePusherBackward");

        m_parts->CylPusher.backward(false);

        EnterState(MoveSafeZAfterPlace);
        return TR_KEEP;
    }

    TaskResult Load2TaskPlace::HandleMoveSafeZAfterPlace(Context& ctx)
    {
        LogStep(ctx, "HandleMoveSafeZAfterPlace");

        m_parts->AxisZ.Move(m_safeZ);

        EnterStateWithTimeout(Complete, m_moveTimeoutMs);
        return TR_NEXT;
    }

    void Load2TaskPlace::LogStep(Context& ctx, const std::string& message)
    {
        std::cout << "[Load2TaskPlace] " << message << std::endl;
        int reqId = ctx.GetParamAs<int>("requestId", 0);
        ctx.SendResult(reqId, "[Step] " + message);
    }
}
