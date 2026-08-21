#include "pch.h"
#include "SetPlate1PLVIExecuteScan.h"
#include "IParamProvider.h"
#include "ParamKeys.h"

using namespace VMF;
using namespace VMF_PLUGIN;

SetPlate1PLVIExecuteScan::SetPlate1PLVIExecuteScan()
    : m_timeoutMoveMs(7000)
    , m_timeoutResultMs(10000)
{
}

SetPlate1PLVIExecuteScan::~SetPlate1PLVIExecuteScan() {}

void SetPlate1PLVIExecuteScan::OnInitialize(VMF::Context& ctx)
{
    // IParamProvider 인터페이스를 통해 실행 파라미터를 조회
    const IParamProvider& provider = static_cast<const IParamProvider&>(ctx);
    
    // ✅ 3단계 리팩토링: Task 이름 기반 파라미터 조회 (Task별 격리)
    // 자신의 Task 이름으로 파라미터를 조회하여 Task 간 격리 강화
    const auto& taskParams = provider.GetTaskParams(GetName());
    
    // ✅ Task 파라미터 조회
    m_timeoutMoveMs = taskParams.GetExecutionParam<int>(ParamKeys::ExecuteScan::TIMEOUT_MOVE_MS, 7000);
    m_timeoutResultMs = taskParams.GetExecutionParam<int>(ParamKeys::ExecuteScan::TIMEOUT_RESULT_MS, 10000);

    // VisionPositions에서 측정 종료 위치 조회 (visionRequestId=3)
    const auto& positions = provider.GetVisionPositions();
    if (positions.size() > 1 && positions[1].visionRequestId == 3)
    {
        m_scanEndPos = positions[1];  // VisionPosition 전체 저장
    }

    EnterState(MoveMeasurementArea);
}

VMF::TaskResult SetPlate1PLVIExecuteScan::OnPoll(VMF::Context& ctx, VMF::IActuator* actuator)
{
    switch (GetState())
    {
    case MoveMeasurementArea:  return HandleMoveMeasurementArea(ctx, actuator);
    case WaitMeasurementArea:  return HandleWaitMeasurementArea(ctx, actuator);
    case RequestResult:        return HandleRequestResult(ctx, actuator);
    case WaitResult:           return HandleWaitResult(ctx, actuator);
    case Complete:             return HandleComplete(ctx, actuator);
    default:                   return TR_ERROR;
    }
}

// 측정 영역 이동 (IActuator 호환 버전)
VMF::TaskResult SetPlate1PLVIExecuteScan::HandleMoveMeasurementArea(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_ExecuteScan: actuator is null.");

    // ✅ VisionPosition 직접 사용 (scanEndY 제거)
    if (actuator->MoveToMeasurementPosition(m_scanEndPos) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_ExecuteScan: MoveToMeasurementPosition failed.");

    EnterStateWithTimeout(WaitMeasurementArea, m_timeoutMoveMs);
    return TR_KEEP;
}

// 측정 영역 도착 확인
VMF::TaskResult SetPlate1PLVIExecuteScan::HandleWaitMeasurementArea(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_ExecuteScan: actuator is null.");

    // ✅ VisionPosition 직접 사용
    if (actuator->IsAtMeasurementPosition(m_scanEndPos) != ActError::ActOk)
    {
        if (IsDeadlineExpired())
            return SetErrorAndReturn(ctx, "PLVI_ExecuteScan: MoveY timeout.");
        return TR_KEEP;
    }

    // 트리거/Laser 해제
    actuator->SetTriggerState(false, 0.0);
    actuator->SetLaserState(0, false);

    EnterState(RequestResult);
    return TR_KEEP;
}

// 결과 요청
VMF::TaskResult SetPlate1PLVIExecuteScan::HandleRequestResult(VMF::Context& ctx, VMF::IActuator* actuator)
{
    auto vp = ctx.GetVisionProcessorInterface();
    if (!vp)
        return SetErrorAndReturn(ctx, "PLVI_ExecuteScan: No VisionProcessor.");

    vp->ClearLatestData(VMF::InspReady);

    if (!ctx.ExecuteVisionCommand(VMF::InspReady))
        return SetErrorAndReturn(ctx, "PLVI_ExecuteScan: InspReady request failed.");

    EnterStateWithTimeout(WaitResult, m_timeoutResultMs);
    return TR_KEEP;
}

// 결과 응답 확인
VMF::TaskResult SetPlate1PLVIExecuteScan::HandleWaitResult(VMF::Context& ctx, VMF::IActuator* actuator)
{
    auto vp = ctx.GetVisionProcessorInterface();
    if (!vp)
        return SetErrorAndReturn(ctx, "PLVI_ExecuteScan: No VisionProcessor.");

    if (!vp->IsValid(VMF::InspReady))
    {
        if (IsDeadlineExpired())
            return SetErrorAndReturn(ctx, "PLVI_ExecuteScan: Result timeout.");
        return TR_KEEP;
    }

    return TR_NEXT;
}

// 완료
VMF::TaskResult SetPlate1PLVIExecuteScan::HandleComplete(VMF::Context& ctx, VMF::IActuator* actuator)
{
    (void)ctx;
    (void)actuator;
    return TR_NEXT;
}
