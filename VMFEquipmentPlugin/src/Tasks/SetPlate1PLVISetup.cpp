#include "pch.h"
#include "SetPlate1PLVISetup.h"
#include "VisionMotionFramework\IParamProvider.h"
#include "PLVIParamKeys.h"

using namespace VMF;
using namespace VMF_PLUGIN;

SetPlate1PLVISetup::SetPlate1PLVISetup()
    : m_timeoutMoveMs(7000)
    , m_triggerIntervalMm(1.8)
{
}

SetPlate1PLVISetup::~SetPlate1PLVISetup() {}

void SetPlate1PLVISetup::OnInitialize(VMF::Context& ctx)
{
    // IParamProvider 인터페이스를 통해 실행 파라미터를 조회
    const IParamProvider& provider = static_cast<const IParamProvider&>(ctx);

    // ✅ 3단계 리팩토링: Task 이름 기반 파라미터 조회 (Task별 격리)
    // 자신의 Task 이름으로 파라미터를 조회하여 Task 간 격리 강화
    const auto& taskParams = provider.GetTaskParams(GetName());

    // ✅ Task 파라미터 조회
    m_timeoutMoveMs = taskParams.GetExecutionParam<int>(PLVI::Setup::TIMEOUT_MOVE_MS, 7000);
    m_triggerIntervalMm = taskParams.GetExecutionParam<double>(PLVI::Setup::TRIGGER_INTERVAL_MM, 1.8);

    // VisionPositions에서 시작 위치 조회 (visionRequestId=1)
    const auto& positions = provider.GetVisionPositions();
    if (!positions.empty() && positions[0].visionRequestId == 1)
    {
        m_startPos = positions[0];  // VisionPosition 전체 저장
    }

    EnterState(MoveSafeZ);
}

VMF::TaskResult SetPlate1PLVISetup::OnPoll(VMF::Context& ctx, VMF::IActuator* actuator)
{
    switch (GetState())
    {
    case MoveSafeZ:        return HandleMoveSafeZ(ctx, actuator);
    case WaitSafeZ:        return HandleWaitSafeZ(ctx, actuator);
    case MoveHorizontalStart:  return HandleMoveHorizontalStart(ctx, actuator);
    case WaitHorizontalStart:  return HandleWaitHorizontalStart(ctx, actuator);
    case SetupTrigger:     return HandleSetupTrigger(ctx, actuator);
    case Complete:         return HandleComplete(ctx, actuator);
    default:               return TR_ERROR;
    }
}

// Z축 안전 위치로 이동
VMF::TaskResult SetPlate1PLVISetup::HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Setup: actuator is null.");

    if (actuator->MoveToZ(m_startPos) != ActError::ActOk)  // VisionPosition 기반 이동
        return SetErrorAndReturn(ctx, "PLVI_Setup: MoveToZ safe failed.");

    EnterStateWithTimeout(WaitSafeZ, m_timeoutMoveMs);
    return TR_KEEP;
}

// Z축 안전 위치 도착 확인
VMF::TaskResult SetPlate1PLVISetup::HandleWaitSafeZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Setup: actuator is null.");

    if (actuator->IsAtZ(m_startPos) != ActError::ActOk)  // VisionPosition 기반 확인
    {
        if (IsDeadlineExpired())
            return SetErrorAndReturn(ctx, "PLVI_Setup: SafeZ timeout.");
        return TR_KEEP;
    }

    EnterStateWithTimeout(MoveHorizontalStart, m_timeoutMoveMs);
    return TR_KEEP;
}

// 수평 시작 위치로 이동
VMF::TaskResult SetPlate1PLVISetup::HandleMoveHorizontalStart(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Setup: actuator is null.");

    // ✅ VisionPosition 직접 사용 (scanStartX/Y/Z 제거)
    if (actuator->MoveToMeasurementPosition(m_startPos) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_Setup: MoveToMeasurementPosition failed.");

    EnterStateWithTimeout(WaitHorizontalStart, m_timeoutMoveMs);
    return TR_KEEP;
}

// 수평 시작 위치 도착 확인
VMF::TaskResult SetPlate1PLVISetup::HandleWaitHorizontalStart(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Setup: actuator is null.");

    // ✅ VisionPosition 직접 사용
    if (actuator->IsAtMeasurementPosition(m_startPos) != ActError::ActOk)
    {
        if (IsDeadlineExpired())
            return SetErrorAndReturn(ctx, "PLVI_Setup: MoveXY timeout.");
        return TR_KEEP;
    }

    EnterState(SetupTrigger);
    return TR_KEEP;
}

// 트리거 신호 발생 설정
VMF::TaskResult SetPlate1PLVISetup::HandleSetupTrigger(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Setup: actuator is null.");

    if (actuator->SetLaserState(0, true) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_Setup: SetLaserState failed.");

    if (actuator->SetTriggerState(true, m_triggerIntervalMm) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_Setup: SetTriggerState failed.");

    EnterState(Complete);
    return TR_KEEP;
}

// 완료
VMF::TaskResult SetPlate1PLVISetup::HandleComplete(VMF::Context& ctx, VMF::IActuator* actuator)
{
    (void)ctx;
    (void)actuator;
    return TR_NEXT;
}
