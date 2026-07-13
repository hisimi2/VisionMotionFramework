#include "pch.h"
#include "CSetPlate1PLVIMoveToScanStartTask.h"

using namespace VMF_PLVI::Task;
using namespace VMF;

CSetPlate1PLVIMoveToScanStartTask::CSetPlate1PLVIMoveToScanStartTask()
	: m_triggerIntervalMm(1.8)
	, m_scanSpeedMmS(100.0)
	, m_moveTimeoutMs(7000)
{}

CSetPlate1PLVIMoveToScanStartTask::~CSetPlate1PLVIMoveToScanStartTask()
{}

void CSetPlate1PLVIMoveToScanStartTask::OnInitialize(Context& ctx)
{
	m_moveTimeoutMs = GetTaskSeqParamAs<int>(ctx, "TIMEOUT_MOVE_MS");
	m_triggerIntervalMm = GetTaskSeqParamAs<double>(ctx, "TRIGGER_INTERVAL_MM");
	m_scanSpeedMmS = GetTaskSeqParamAs<double>(ctx, "SCAN_SPEED_MM_S");

	EnterState(MoveSafeZ);
}

TaskResult CSetPlate1PLVIMoveToScanStartTask::OnPoll(
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

TaskResult CSetPlate1PLVIMoveToScanStartTask::HandleMoveSafeZ(
	Context& ctx, IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: actuator is null.");

	if (!PeekTaskVisionPosition(m_measurepos))
		return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: No scan position.");

	m_scanStartPos = m_measurepos.pos;  // [0]=X, [1]=Y, [2]=VisionZ

	if (actuator->MoveZ(0.0) != ActOk)
		return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: MoveZ safe failed.");

	EnterStateWithTimeout(WaitSafeZ, m_moveTimeoutMs);
	return TR_KEEP;
}


TaskResult CSetPlate1PLVIMoveToScanStartTask::HandleWaitSafeZ(
	Context& ctx, IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: actuator is null.");

	if (actuator->isMoveZ(0.0) != ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: SafeZ timeout.");
		return TR_KEEP;
	}

	EnterStateWithTimeout(MoveScanStartXY, m_moveTimeoutMs);
	return TR_KEEP;
}


TaskResult CSetPlate1PLVIMoveToScanStartTask::HandleMoveScanStartXY(
	Context& ctx, IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: actuator is null.");

	MotionCommand cmd;
	cmd.Set("X", m_scanStartPos[0]);
	cmd.Set("Y", m_scanStartPos[1]);
	cmd.pitch = Narrow;

	if (actuator->Move(cmd) != ActOk)
		return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: MoveXY failed.");

	EnterStateWithTimeout(WaitScanStartXY, m_moveTimeoutMs);
	return TR_KEEP;
}


TaskResult CSetPlate1PLVIMoveToScanStartTask::HandleWaitScanStartXY(
	Context& ctx, IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: actuator is null.");

	MotionCommand cmd;
	cmd.Set("X", m_scanStartPos[0]);
	cmd.Set("Y", m_scanStartPos[1]);
	cmd.pitch = Narrow;

	if (actuator->isMove(cmd) != ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: MoveXY timeout.");
		return TR_KEEP;
	}

	EnterStateWithTimeout(MoveVisionZ, m_moveTimeoutMs);
	return TR_KEEP;
}

TaskResult CSetPlate1PLVIMoveToScanStartTask::HandleMoveVisionZ(
	Context& ctx, IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: actuator is null.");

	if (actuator->MoveZ(m_scanStartPos[2]) != ActOk)
		return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: MoveVisionZ failed.");

	EnterStateWithTimeout(WaitVisionZ, m_moveTimeoutMs);
	return TR_KEEP;
}


TaskResult CSetPlate1PLVIMoveToScanStartTask::HandleWaitVisionZ(
	Context& ctx, IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: actuator is null.");

	if (actuator->isMoveZ(m_scanStartPos[2]) != ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: VisionZ timeout.");
		return TR_KEEP;
	}

	EnterState(SetupTrigger);
	return TR_KEEP;
}

TaskResult CSetPlate1PLVIMoveToScanStartTask::HandleSetupTrigger(
	Context& ctx, IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: actuator is null.");

	if (actuator->SetLaserState(0,true) != ActOk)
		return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: LaserOn failed.");

	if (actuator->SetTriggerState(0, true) != ActOk)
		return SetErrorAndReturn(ctx, "PLVI_MoveToScanStart: SetTrigger failed.");

	EnterState(Complete);
	return TR_KEEP;
}

TaskResult CSetPlate1PLVIMoveToScanStartTask::HandleComplete(
	Context& ctx, IActuator* actuator)
{
	(void)ctx;
	(void)actuator;
	return TR_NEXT;
}
