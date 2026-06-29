#include "pch.h"
#include "SampleMoveToStartPositionTask.h"
#include "DefineVAT.h"

using namespace VMF_Sample::Task;
using namespace VMF;

SampleMoveToStartPositionTask::SampleMoveToStartPositionTask()
	: m_moveTimeoutMs(7000)
{}

SampleMoveToStartPositionTask::~SampleMoveToStartPositionTask()
{}

void SampleMoveToStartPositionTask::OnInitialize(Context& ctx)
{
	const int timeoutMs = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_TIMEOUT_MS, m_moveTimeoutMs);
	if (timeoutMs > 0)
		m_moveTimeoutMs = timeoutMs;

	EnterState(MoveSafeZ);
}

TaskResult SampleMoveToStartPositionTask::OnPoll(Context& ctx, IActuator* actuator)
{
	switch (GetState())
	{
	case MoveSafeZ:            return HandleMoveSafeZ(ctx, actuator);
	case MoveOrigin:           return HandleMoveOrigin(ctx, actuator);
	case MoveTargetPositionXY: return HandleMoveTargetPositionXY(ctx, actuator);
	case MoveTargetPositionZ:  return HandleMoveTargetPositionZ(ctx, actuator);
	case CompleteMove:         return HandleCompleteMove(ctx, actuator);
	case CS_ERROR:
	default:                   return TR_ERROR;
	}
}

TaskResult SampleMoveToStartPositionTask::HandleMoveSafeZ(
	Context& ctx, IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	VisionPosition visionPosition;
	if (!ctx.PeekVisionPosition(visionPosition))
		return SetErrorAndReturn(ctx, "MoveToStartPosition: Get Position Failed.");

	m_targetPosition = visionPosition.pos;

	if (actuator->MoveZ(0.0) != ActOk)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveToSafeZ failed.");

	EnterStateWithTimeout(MoveOrigin, m_moveTimeoutMs);
	return TR_KEEP;
}

TaskResult SampleMoveToStartPositionTask::HandleMoveOrigin(
	Context& ctx, IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	if (actuator->isMoveZ(0.0) != ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "MoveToStartPosition: SafeZ timeout.");
		return TR_KEEP;
	}

	// Origin XY 이동 — X=0, Y=0
	// !!! 수정 필요: 장비의 Origin 위치/축 이름으로 변경 !!!
	MotionCommand cmd;
	cmd.Set("X", 0.0);
	cmd.Set("Y", 0.0);
	cmd.pitch = Narrow;

	if (actuator->Move(cmd) != ActOk)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveToOrigin failed.");

	EnterStateWithTimeout(MoveTargetPositionXY, m_moveTimeoutMs);
	return TR_KEEP;
}

TaskResult SampleMoveToStartPositionTask::HandleMoveTargetPositionXY(
	Context& ctx, IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	// Origin 도달 확인
	MotionCommand originCmd;
	originCmd.Set("X", 0.0);
	originCmd.Set("Y", 0.0);
	originCmd.pitch = Narrow;

	if (actuator->isMove(originCmd) != ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "MoveToStartPosition: Origin timeout.");
		return TR_KEEP;
	}

	MotionCommand targetCmd;
	targetCmd.Set("X", m_targetPosition[0]);
	targetCmd.Set("Y", m_targetPosition[1]);
	targetCmd.pitch = Narrow;

	// !!! 수정 필요: 장비의 Table 축이 있다면 추가 !!!
	if (m_targetPosition.size() > 3)
	{
		targetCmd.Set("TABLE1", m_targetPosition[3]);
		targetCmd.Set("TABLE2", m_targetPosition[4]);
	}

	if (actuator->Move(targetCmd) != ActOk)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveToFocusInspPos (XY) failed.");

	EnterStateWithTimeout(MoveTargetPositionZ, m_moveTimeoutMs);
	return TR_KEEP;
}

TaskResult SampleMoveToStartPositionTask::HandleMoveTargetPositionZ(
	Context& ctx, IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	MotionCommand targetCmd;
	targetCmd.Set("X", m_targetPosition[0]);
	targetCmd.Set("Y", m_targetPosition[1]);
	targetCmd.pitch = Narrow;

	if (m_targetPosition.size() > 3)
	{
		targetCmd.Set("TABLE1", m_targetPosition[3]);
		targetCmd.Set("TABLE2", m_targetPosition[4]);
	}

	if (actuator->isMove(targetCmd) != ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "MoveToStartPosition: Move XY timeout.");
		return TR_KEEP;
	}

	if (actuator->MoveZ(m_targetPosition[2]) != ActOk)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveToFocusInspPos (Z) failed.");

	EnterStateWithTimeout(CompleteMove, m_moveTimeoutMs);
	return TR_KEEP;
}

TaskResult SampleMoveToStartPositionTask::HandleCompleteMove(
	Context& ctx, IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	if (actuator->isMoveZ(m_targetPosition[2]) != ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveZ timeout.");
		return TR_KEEP;
	}

	EnterState(MoveSafeZ);  // 다음 호출을 위한 초기 상태 복귀
	return TR_NEXT;
}
