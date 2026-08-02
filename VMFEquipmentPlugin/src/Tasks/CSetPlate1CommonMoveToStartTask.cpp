#include "pch.h"
#include "CSetPlate1CommonMoveToStartTask.h"

using namespace VMF;
using namespace VMF_PLUGIN;

CSetPlate1CommonMoveToStartTask::CSetPlate1CommonMoveToStartTask()
	: m_scanSpeedMmS(100.0)
	, m_moveTimeoutMs(7000)
{}

CSetPlate1CommonMoveToStartTask::~CSetPlate1CommonMoveToStartTask()
{}

void CSetPlate1CommonMoveToStartTask::OnInitialize(Context& ctx)
{
	m_moveTimeoutMs = GetTaskSeqParamAs<int>(ctx, "TIMEOUT_MOVE_MS");
	m_scanSpeedMmS = GetTaskSeqParamAs<double>(ctx, "SCAN_SPEED_MM_S");

	EnterState(MoveSafeZ);
}

TaskResult CSetPlate1CommonMoveToStartTask::OnPoll(
	Context& ctx, IActuator* actuator)
{
	switch (GetState())
	{
	case MoveSafeZ:        return HandleMoveSafeZ(ctx, actuator);
	case WaitSafeZ:        return HandleWaitSafeZ(ctx, actuator);
	case MoveScanStartXY:  return HandleMoveScanStartXY(ctx, actuator);
	case WaitScanStartXY:  return HandleWaitScanStartXY(ctx, actuator);
	case Complete:         return HandleComplete(ctx, actuator);
	default:               return TR_ERROR;
	}
}

// Z축을 안전 위치로 이동
TaskResult CSetPlate1CommonMoveToStartTask::HandleMoveSafeZ(
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


TaskResult CSetPlate1CommonMoveToStartTask::HandleWaitSafeZ(
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


TaskResult CSetPlate1CommonMoveToStartTask::HandleMoveScanStartXY(
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


TaskResult CSetPlate1CommonMoveToStartTask::HandleWaitScanStartXY(
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

TaskResult CSetPlate1CommonMoveToStartTask::HandleComplete(
	Context& ctx, IActuator* actuator)
{
	(void)ctx;
	(void)actuator;
	return TR_NEXT;
}

