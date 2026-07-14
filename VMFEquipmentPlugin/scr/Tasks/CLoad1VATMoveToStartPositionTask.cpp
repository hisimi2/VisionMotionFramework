#include "pch.h"
#include "CLoad1VATMoveToStartPositionTask.h"
#include "DefineVAT.h"

using namespace VAT_LOAD1::Task;

CLoad1VATMoveToStartPositionTask::CLoad1VATMoveToStartPositionTask()
	: m_moveTimeoutMs(7000)
{
}

CLoad1VATMoveToStartPositionTask::~CLoad1VATMoveToStartPositionTask()
{
}

void CLoad1VATMoveToStartPositionTask::OnInitialize(VMF::Context& ctx)
{
	const int timeoutMs = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_TIMEOUT_MS, m_moveTimeoutMs);
	if (timeoutMs > 0)
		m_moveTimeoutMs = timeoutMs;

	EnterState(MoveSafeZ);
}

VMF::TaskResult CLoad1VATMoveToStartPositionTask::OnPoll(
	VMF::Context& ctx,
	VMF::IActuator* actuator)
{
	switch (GetState())
	{
	case MoveSafeZ:            return HandleMoveSafeZ(ctx, actuator);
	case MoveOrigin:           return HandleMoveOrigin(ctx, actuator);
	case MoveTargetPositionXY: return HandleMoveTargetPositionXY(ctx, actuator);
	case MoveTargetPositionZ:  return HandleMoveTargetPositionZ(ctx, actuator);
	case CompleteMove:         return HandleCompleteMove(ctx, actuator);
	case CS_ERROR:             return VMF::TR_ERROR;
	default:                   return VMF::TR_ERROR;
	}
}

VMF::TaskResult CLoad1VATMoveToStartPositionTask::HandleMoveSafeZ(
	VMF::Context& ctx,
	VMF::IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

    /*
	VMF::VisionPosition visionPosition;
	if (!ctx.PeekVisionPosition(visionPosition))
		return SetErrorAndReturn(ctx, "MoveToStartPosition: Get Position Failed.");

	m_targetPosition = visionPosition.pos;
    */

	if (actuator->MoveZ(0.0) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveToSafeZ failed.");

	EnterStateWithTimeout(MoveOrigin, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATMoveToStartPositionTask::HandleMoveOrigin(
	VMF::Context& ctx,
	VMF::IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	if (actuator->isMoveZ(0.0) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "MoveToStartPosition: SafeZ timeout.");
		return VMF::TR_KEEP;
	}

	VMF::MotionCommand cmd;
	cmd.Set("X", 0.0);
	cmd.Set("Y", 0.0);
	cmd.pitch = VMF::Narrow;

	if (actuator->Move(cmd) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveToOrigin failed.");

	EnterStateWithTimeout(MoveTargetPositionXY, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATMoveToStartPositionTask::HandleMoveTargetPositionXY(
	VMF::Context& ctx,
	VMF::IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	VMF::MotionCommand originCmd;
	originCmd.Set("X", 0.0);
	originCmd.Set("Y", 0.0);
	originCmd.pitch = VMF::Narrow;

	if (actuator->isMove(originCmd) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "MoveToStartPosition: SafeZ timeout.");
		return VMF::TR_KEEP;
	}

	VMF::MotionCommand cmd;
	cmd.Set("X", m_targetPosition[0]);
	cmd.Set("Y", m_targetPosition[1]);
	cmd.pitch = VMF::Narrow;

	if (actuator->Move(cmd) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveToFocusInspPos (XY) failed.");

	EnterStateWithTimeout(MoveTargetPositionZ, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATMoveToStartPositionTask::HandleMoveTargetPositionZ(
	VMF::Context& ctx,
	VMF::IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	VMF::MotionCommand cmd;
	cmd.Set("X", m_targetPosition[0]);
	cmd.Set("Y", m_targetPosition[1]);
	cmd.pitch = VMF::Narrow;

	if (actuator->isMove(cmd) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "MoveToStartPosition: Move timeout.");
		return VMF::TR_KEEP;
	}

	if (actuator->MoveZ(m_targetPosition[2]) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveToFocusInspPos (Z) failed.");

	EnterStateWithTimeout(CompleteMove, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATMoveToStartPositionTask::HandleCompleteMove(
	VMF::Context& ctx,
	VMF::IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	if (actuator->isMoveZ(m_targetPosition[2]) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveZ timeout.");
		return VMF::TR_KEEP;
	}

	EnterState(MoveSafeZ);
	return VMF::TR_NEXT;
}
