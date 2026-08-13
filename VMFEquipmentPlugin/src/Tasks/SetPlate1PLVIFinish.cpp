#include "pch.h"
#include "SetPlate1PLVIFinish.h"
#include "IParamProvider.h"

using namespace VMF;
using namespace VMF_PLUGIN;

SetPlate1PLVIFinish::SetPlate1PLVIFinish()
    : m_moveTimeoutMs(7000)
{
}

SetPlate1PLVIFinish::~SetPlate1PLVIFinish() {}

void SetPlate1PLVIFinish::OnInitialize(VMF::Context& ctx)
{
    // IParamProvider 인터페이스를 통해 Finish Task 전용 파라미터를 조회
    const IParamProvider& provider = static_cast<const IParamProvider&>(ctx);
    auto finishParams = provider.GetFinishParams();
    m_moveTimeoutMs = finishParams.timeoutMoveMs;
    EnterState(MoveSafeZ);
}

VMF::TaskResult SetPlate1PLVIFinish::OnPoll(VMF::Context& ctx, VMF::IActuator* actuator)
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
VMF::TaskResult SetPlate1PLVIFinish::HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Finish: actuator is null.");

    if (actuator->MoveToZ(0.0) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_Finish: MoveToZ safe failed.");

    EnterStateWithTimeout(WaitSafeZ, m_moveTimeoutMs);
    return TR_KEEP;
}

// Z축 안전 위치 도착 확인
VMF::TaskResult SetPlate1PLVIFinish::HandleWaitSafeZ(VMF::Context& ctx, VMF::IActuator* actuator)
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
VMF::TaskResult SetPlate1PLVIFinish::HandleMoveHome(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Finish: actuator is null.");

    if (actuator->MoveToHomePosition() != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_Finish: MoveToHomePosition failed.");

    EnterStateWithTimeout(WaitHome, m_moveTimeoutMs);
    return TR_KEEP;
}

// 대기 위치 도착 확인
VMF::TaskResult SetPlate1PLVIFinish::HandleWaitHome(VMF::Context& ctx, VMF::IActuator* actuator)
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
VMF::TaskResult SetPlate1PLVIFinish::HandleComplete(VMF::Context& ctx, VMF::IActuator* actuator)
{
    (void)ctx;
    (void)actuator;
    return TR_NEXT;
}
