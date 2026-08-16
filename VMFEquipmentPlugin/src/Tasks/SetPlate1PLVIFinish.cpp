#include "pch.h"
#include "SetPlate1PLVIFinish.h"
#include "IParamProvider.h"
#include "ParamKeys.h"

using namespace VMF;
using namespace VMF_PLUGIN;

SetPlate1PLVIFinish::SetPlate1PLVIFinish()
    : m_params()  // 구조체 기본값으로 초기화
{
}

SetPlate1PLVIFinish::~SetPlate1PLVIFinish() {}

void SetPlate1PLVIFinish::OnInitialize(VMF::Context& ctx)
{
    // IParamProvider 인터페이스를 통해 실행 파라미터를 조회
    const IParamProvider& provider = static_cast<const IParamProvider&>(ctx);
    
    // ✅ 3단계 리팩토링: Task 이름 기반 파라미터 조회 (Task별 격리)
    // 자신의 Task 이름으로 파라미터를 조회하여 Task 간 격리 강화
    const auto& taskParams = provider.GetTaskParams(GetName());
    
    // ✅ Task별 파라미터 구조체 사용
    m_params.timeoutMoveMs = taskParams.GetExecutionParam<int>(ParamKeys::Finish::TIMEOUT_MOVE_MS, 7000);
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

    EnterStateWithTimeout(WaitSafeZ, m_params.timeoutMoveMs);
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

    EnterStateWithTimeout(MoveHome, m_params.timeoutMoveMs);
    return TR_KEEP;
}

// 대기 위치로 이동
VMF::TaskResult SetPlate1PLVIFinish::HandleMoveHome(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Finish: actuator is null.");

    if (actuator->MoveToHomePosition() != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_Finish: MoveToHomePosition failed.");

    EnterStateWithTimeout(WaitHome, m_params.timeoutMoveMs);
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
