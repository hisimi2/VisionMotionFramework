#include "pch.h"
#include "SetPlate1PLVIFinish.h"
#include "IParamProvider.h"
#include "ParamKeys.h"

using namespace VMF;
using namespace VMF_PLUGIN;

SetPlate1PLVIFinish::SetPlate1PLVIFinish()
    : m_timeoutMoveMs(7000)
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
    
    // ✅ Task 파라미터 조회
    m_timeoutMoveMs = taskParams.GetExecutionParam<int>(ParamKeys::Finish::TIMEOUT_MOVE_MS, 7000);

    // VisionPositions에서 안전 Z/홈 위치 조회 (visionRequestId=4, 5)
    const auto& positions = provider.GetVisionPositions();
    if (positions.size() > 0 && positions[0].visionRequestId == 4)
    {
        m_safeZPos = positions[0];  // VisionPosition 전체 저장
    }
    if (positions.size() > 1 && positions[1].visionRequestId == 5)
    {
        m_homePos = positions[1];  // VisionPosition 전체 저장
    }

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

    // ✅ VisionPosition 기반 이동 (MoveToZ double → VisionPosition)
    if (actuator->MoveToZ(m_safeZPos) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_Finish: MoveToZ safe failed.");

    EnterStateWithTimeout(WaitSafeZ, m_timeoutMoveMs);
    return TR_KEEP;
}

// Z축 안전 위치 도착 확인
VMF::TaskResult SetPlate1PLVIFinish::HandleWaitSafeZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Finish: actuator is null.");

    // ✅ VisionPosition 기반 확인 (IsAtZ double → VisionPosition)
    if (actuator->IsAtZ(m_safeZPos) != ActError::ActOk)
    {
        if (IsDeadlineExpired())
            return SetErrorAndReturn(ctx, "PLVI_Finish: SafeZ timeout.");
        return TR_KEEP;
    }

    EnterStateWithTimeout(MoveHome, m_timeoutMoveMs);
    return TR_KEEP;
}

// 대기 위치로 이동
VMF::TaskResult SetPlate1PLVIFinish::HandleMoveHome(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Finish: actuator is null.");

    // ✅ VisionPosition 기반 이동 (MoveToHomePosition → VisionPosition 파라미터)
    if (actuator->MoveToHomePosition(m_homePos) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_Finish: MoveToHomePosition failed.");

    EnterStateWithTimeout(WaitHome, m_timeoutMoveMs);
    return TR_KEEP;
}

// 대기 위치 도착 확인
VMF::TaskResult SetPlate1PLVIFinish::HandleWaitHome(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Finish: actuator is null.");

    // ✅ VisionPosition 기반 확인 (IsAtHomePosition → VisionPosition 파라미터)
    if (actuator->IsAtHomePosition(m_homePos) != ActError::ActOk)
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
