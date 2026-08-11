#include "pch.h"
#include "CSetPlate1PLVIFinishTask.h"

using namespace VMF;
using namespace VMF_PLUGIN;

CSetPlate1PLVIFinishTask::CSetPlate1PLVIFinishTask()
    : m_moveTimeoutMs(7000)
{
}

CSetPlate1PLVIFinishTask::~CSetPlate1PLVIFinishTask() {}

void CSetPlate1PLVIFinishTask::OnInitialize(VMF::Context& ctx)
{
    m_moveTimeoutMs = GetTaskSeqParamAs<int>(ctx, "TIMEOUT_MOVE_MS");
    EnterState(MoveSafeZ);
}

VMF::TaskResult CSetPlate1PLVIFinishTask::OnPoll(VMF::Context& ctx, VMF::IActuator* actuator)
{
    switch (GetState())
    {
    case MoveSafeZ:        return HandleMoveSafeZ(ctx, actuator);
    case WaitSafeZ:        return HandleWaitSafeZ(ctx, actuator);
    case MoveHome:         return HandleMoveHome(ctx, actuator);
    case WaitHome:         return HandleWaitHome(ctx, actuator);
    case Complete:         return HandleComplete(ctx, actuator);
    default:               return TR_ERROR;
    }
}

// Z축 안전 위치로 이동
VMF::TaskResult CSetPlate1PLVIFinishTask::HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Finish: actuator is null.");

    if (actuator->MoveToZ(0.0) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_Finish: MoveToZ safe failed.");

    EnterStateWithTimeout(WaitSafeZ, m_moveTimeoutMs);
    return TR_KEEP;
}

// Z축 안전 위치 도착 확인
VMF::TaskResult CSetPlate1PLVIFinishTask::HandleWaitSafeZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Finish: actuator is null.");

    if (actuator->IsAtZ(0.0) != ActError::ActOk)
    {
        if (IsDeadlineExpired())
            return SetErrorAndReturn(ctx, "PLVI_Finish: SafeZ timeout.");
        return TR_KEEP;
    }

    EnterStateWithTimeout(MoveHome, m_moveTimeoutMs);
    return TR_KEEP;
}

// 대기 위치로 이동
VMF::TaskResult CSetPlate1PLVIFinishTask::HandleMoveHome(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Finish: actuator is null.");

    if (actuator->MoveToHomePosition() != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_Finish: MoveToHomePosition failed.");

    EnterStateWithTimeout(WaitHome, m_moveTimeoutMs);
    return TR_KEEP;
}

// 대기 위치 도착 확인
VMF::TaskResult CSetPlate1PLVIFinishTask::HandleWaitHome(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Finish: actuator is null.");

    if (actuator->IsAtHomePosition() != ActError::ActOk)
    {
        if (IsDeadlineExpired())
            return SetErrorAndReturn(ctx, "PLVI_Finish: Home timeout.");
        return TR_KEEP;
    }

    return TR_NEXT;
}

// 완료
VMF::TaskResult CSetPlate1PLVIFinishTask::HandleComplete(VMF::Context& ctx, VMF::IActuator* actuator)
{
    (void)ctx;
    (void)actuator;
    return TR_NEXT;
}
