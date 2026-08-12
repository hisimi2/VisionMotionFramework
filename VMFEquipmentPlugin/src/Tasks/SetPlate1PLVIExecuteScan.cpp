#include "pch.h"
#include "SetPlate1PLVIExecuteScan.h"

using namespace VMF;
using namespace VMF_PLUGIN;

SetPlate1PLVIExecuteScan::SetPlate1PLVIExecuteScan()
    : m_scanEndY(200.0), m_timeoutMoveMs(7000), m_timeoutResultMs(10000)
{
}

SetPlate1PLVIExecuteScan::~SetPlate1PLVIExecuteScan() {}

void SetPlate1PLVIExecuteScan::OnInitialize(VMF::Context& ctx)
{
    // Context를 통해 파라미터를 전달받아 사용
    m_timeoutMoveMs = ctx.GetTaskParamAs<int>("TIMEOUT_MOVE_MS", 7000);
    m_timeoutResultMs = ctx.GetTaskParamAs<int>("TIMEOUT_RESULT_MS", 10000);
    m_scanEndY = ctx.GetTaskParamAs<double>("SCAN_END_Y", 200.0);

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

    // IActuator에는 MoveToY가 없으므로 MeasurementPosition 사용
    VisionPosition targetPos;
    targetPos.pos = { 0.0, m_scanEndY, 0.0 };  // X=0, Y=scanEndY, Z=0

    if (actuator->MoveToMeasurementPosition(targetPos) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_ExecuteScan: MoveToMeasurementPosition failed.");

    EnterStateWithTimeout(WaitMeasurementArea, m_timeoutMoveMs);
    return TR_KEEP;
}

// 측정 영역 도착 확인
VMF::TaskResult SetPlate1PLVIExecuteScan::HandleWaitMeasurementArea(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_ExecuteScan: actuator is null.");

    VisionPosition targetPos;
    targetPos.pos = { 0.0, m_scanEndY, 0.0 };

    if (actuator->IsAtMeasurementPosition(targetPos) != ActError::ActOk)
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
