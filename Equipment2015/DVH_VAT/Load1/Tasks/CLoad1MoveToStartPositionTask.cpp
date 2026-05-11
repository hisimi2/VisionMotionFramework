#include "StdAfx.h"
#include "CLoad1MoveToStartPositionTask.h"
#include "DVH_VAT/DefineVAT.h"

using namespace VAT_LOAD1::Task;

CLoad1MoveToStartPositionTask::CLoad1MoveToStartPositionTask()
	: m_moveTimeoutMs(7000)
{
}

CLoad1MoveToStartPositionTask::~CLoad1MoveToStartPositionTask()
{
}

void CLoad1MoveToStartPositionTask::OnInitialize(VMF::VAT_Context& ctx)
{
	const int timeoutMs = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_TIMEOUT_MS, m_moveTimeoutMs);
	if (timeoutMs > 0)
		m_moveTimeoutMs = timeoutMs;

	EnterState(MoveSafeZ);
}

VMF::TaskResult CLoad1MoveToStartPositionTask::OnPoll(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	switch (GetState())
	{
	case MoveSafeZ:            return HandleMoveSafeZ(ctx, actuator);
	case MoveOrigin:           return HandleMoveOrigin(ctx, actuator);
	case MoveTargetPositionXY: return HandleMoveTargetPositionXY(ctx, actuator);
	case MoveTargetPositionZ:  return HandleMoveTargetPositionZ(ctx, actuator);
	case CompleteMove:         return HandleCompleteMove(ctx, actuator);
	case CS_ERROR:            return VMF::TR_ERROR;
	default:                  return VMF::TR_ERROR;
	}
}

VMF::TaskResult CLoad1MoveToStartPositionTask::HandleMoveSafeZ(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	VMF::VisionPosition visionPosition;
	if (!ctx.PeekVisionPosition(visionPosition))
	{
		return SetErrorAndReturn(ctx, "MoveToStartPosition: Get Position Failed.");
	}

	m_targetPosition = visionPosition.pos;

	if (actuator->MoveZ(0.0) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveToSafeZ failed.");

	EnterStateWithTimeout(MoveOrigin, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1MoveToStartPositionTask::HandleMoveOrigin(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	if (actuator->isMoveZ(0.0) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "MoveToStartPosition: SafeZ timeout.");

		return VMF::TR_KEEP;
	}

	std::vector<double> originXY;
	originXY.push_back(0.0);
	originXY.push_back(0.0);

	if (actuator->Move(originXY, VMF::Narrow) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveToOrigin failed.");

	EnterStateWithTimeout(MoveTargetPositionXY, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1MoveToStartPositionTask::HandleMoveTargetPositionXY(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	std::vector<double> originXY;
	originXY.push_back(0.0);
	originXY.push_back(0.0);

	if (actuator->isMove(originXY, VMF::Narrow) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "MoveToStartPosition: SafeZ timeout.");

		return VMF::TR_KEEP;
	}

	if (actuator->Move(m_targetPosition, VMF::Narrow) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveToFocusInspPos (XY) failed.");

	EnterStateWithTimeout(MoveTargetPositionZ, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1MoveToStartPositionTask::HandleMoveTargetPositionZ(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	if (actuator->isMove(m_targetPosition, VMF::Narrow) != VMF::ActOk)
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

VMF::TaskResult CLoad1MoveToStartPositionTask::HandleCompleteMove(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
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

