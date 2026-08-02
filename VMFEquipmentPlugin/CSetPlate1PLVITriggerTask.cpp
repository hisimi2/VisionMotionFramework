#include "pch.h"
#include "CSetPlate1PLVITriggerTask.h"

using namespace VMF;
using namespace VMF_PLUGIN;

CSetPlate1PLVITriggerTask::CSetPlate1PLVITriggerTask()
    : m_triggerIntervalMm(1.8)
    , m_scanSpeedMmS(100.0)
    , m_moveTimeoutMs(7000)
{
}

CSetPlate1PLVITriggerTask::~CSetPlate1PLVITriggerTask()
{
}

void CSetPlate1PLVITriggerTask::OnInitialize(Context& ctx)
{
    m_moveTimeoutMs = GetTaskSeqParamAs<int>(ctx, "TIMEOUT_MOVE_MS");
    m_triggerIntervalMm = GetTaskSeqParamAs<double>(ctx, "TRIGGER_INTERVAL_MM");
    m_scanSpeedMmS = GetTaskSeqParamAs<double>(ctx, "SCAN_SPEED_MM_S");

    EnterState(MoveSafeZ);
}

TaskResult CSetPlate1PLVITriggerTask::OnPoll(
    Context& ctx, IActuator* actuator)
{
    switch (GetState())
    {
    case MoveSafeZ:        return HandleMoveSafeZ(ctx, actuator);
    case WaitSafeZ:        return HandleWaitSafeZ(ctx, actuator);
    case MoveScanStartXY:  return HandleMoveScanStartXY(ctx, actuator);
    case WaitScanStartXY:  return HandleWaitScanStartXY(ctx, actuator);
    case MoveVisionZ:      return HandleMoveVisionZ(ctx, actuator);
    case WaitVisionZ:      return HandleWaitVisionZ(ctx, actuator);
    case SetupTrigger:     return HandleSetupTrigger(ctx, actuator);
    case Complete:         return HandleComplete(ctx, actuator);
    case CS_ERROR:
    default:               return TR_ERROR;
    }
}

// Z축을 안전 위치로 이동
TaskResult CSetPlate1PLVITriggerTask::HandleMoveSafeZ(
    Context& ctx, IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: actuator is null.");

    if (!PeekTaskVisionPosition(m_measurepos))
        return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: No scan position.");

    m_scanStartPos = m_measurepos.pos;  // [0]=X, [1]=Y, [2]=VisionZ

    // Z축을 안전 위치(0.0)로 이동
    if (actuator->MoveToZ(0.0) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: MoveToZ safe failed.");

    EnterStateWithTimeout(WaitSafeZ, m_moveTimeoutMs);
    return TR_KEEP;
}


TaskResult CSetPlate1PLVITriggerTask::HandleWaitSafeZ(
    Context& ctx, IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: actuator is null.");

    // Z축이 안전 위치(0.0)에 도달했는지 확인
    if (actuator->IsAtZ(0.0) != ActError::ActOk)
    {
        if (IsDeadlineExpired())
            return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: SafeZ timeout.");
        return TR_KEEP;
    }

    EnterStateWithTimeout(MoveScanStartXY, m_moveTimeoutMs);
    return TR_KEEP;
}


TaskResult CSetPlate1PLVITriggerTask::HandleMoveScanStartXY(
    Context& ctx, IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: actuator is null.");

    // VisionPosition 생성 (X, Y만 사용, Z는 0으로 설정)
    VisionPosition targetPos;
    targetPos.pos = { m_scanStartPos[0], m_scanStartPos[1], 0.0 };

    // 측정 위치(X, Y)로 이동
    if (actuator->MoveToMeasurementPosition(targetPos) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: MoveToMeasurementPosition failed.");

    EnterStateWithTimeout(WaitScanStartXY, m_moveTimeoutMs);
    return TR_KEEP;
}


TaskResult CSetPlate1PLVITriggerTask::HandleWaitScanStartXY(
    Context& ctx, IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: actuator is null.");

    // VisionPosition 생성 (X, Y만 사용)
    VisionPosition targetPos;
    targetPos.pos = { m_scanStartPos[0], m_scanStartPos[1], 0.0 };

    // 측정 위치(X, Y) 도달 확인
    if (actuator->IsAtMeasurementPosition(targetPos) != ActError::ActOk)
    {
        if (IsDeadlineExpired())
            return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: MoveXY timeout.");
        return TR_KEEP;
    }

    EnterStateWithTimeout(MoveVisionZ, m_moveTimeoutMs);
    return TR_KEEP;
}

TaskResult CSetPlate1PLVITriggerTask::HandleMoveVisionZ(
    Context& ctx, IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: actuator is null.");

    // Vision Z축으로 이동
    if (actuator->MoveToZ(m_scanStartPos[2]) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: MoveToZ VisionZ failed.");

    EnterStateWithTimeout(WaitVisionZ, m_moveTimeoutMs);
    return TR_KEEP;
}


TaskResult CSetPlate1PLVITriggerTask::HandleWaitVisionZ(
    Context& ctx, IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: actuator is null.");

    // Vision Z축 도달 확인
    if (actuator->IsAtZ(m_scanStartPos[2]) != ActError::ActOk)
    {
        if (IsDeadlineExpired())
            return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: VisionZ timeout.");
        return TR_KEEP;
    }

    EnterState(SetupTrigger);
    return TR_KEEP;
}

TaskResult CSetPlate1PLVITriggerTask::HandleSetupTrigger(
    Context& ctx, IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: actuator is null.");

    // 레이저 켜기 (채널 0)
    if (actuator->SetLaserState(0, true) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: SetLaserState failed.");

    // 트리거 설정 (채널 0, 활성화, 간격)
    if (actuator->SetTriggerState(true, m_triggerIntervalMm) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: SetTriggerState failed.");

    EnterState(Complete);
    return TR_KEEP;
}

TaskResult CSetPlate1PLVITriggerTask::HandleComplete(
    Context& ctx, IActuator* actuator)
{
    (void)ctx;
    (void)actuator;
    return TR_NEXT;
}
