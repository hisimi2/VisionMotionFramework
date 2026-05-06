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

void CLoad1MoveToStartPositionTask::OnInitialize(DVH_VAT::VAT_Context& ctx)
{
	const int timeoutMs = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_TIMEOUT_MS, m_moveTimeoutMs);
	if (timeoutMs > 0)
		m_moveTimeoutMs = timeoutMs;

	EnterState(MoveSafeZ);
}

DVH_VAT::TaskResult CLoad1MoveToStartPositionTask::OnPoll(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	switch (GetState())
	{
	case MoveSafeZ:            return HandleMoveSafeZ(ctx, actuator);
	case MoveOrigin:           return HandleMoveOrigin(ctx, actuator);
	case MoveTargetPositionXY: return HandleMoveTargetPositionXY(ctx, actuator);
	case MoveTargetPositionZ:  return HandleMoveTargetPositionZ(ctx, actuator);
	case CompleteMove:         return HandleCompleteMove(ctx, actuator);
	case CS_ERROR:            return DVH_VAT::TR_ERROR;
	default:                  return DVH_VAT::TR_ERROR;
	}
}

DVH_VAT::TaskResult CLoad1MoveToStartPositionTask::HandleMoveSafeZ(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	DVH_VAT::VisionPosition visionPosition;
	if (!ctx.PeekVisionPosition(visionPosition))
	{
		return SetErrorAndReturn(ctx, "MoveToStartPosition: Get Position Failed.");
	}

	m_targetPosition = visionPosition.pos;

	if (actuator->MoveZ(0.0) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveToSafeZ failed.");

	EnterStateWithTimeout(MoveOrigin, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1MoveToStartPositionTask::HandleMoveOrigin(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	if (actuator->isMoveZ(0.0) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "MoveToStartPosition: SafeZ timeout.");

		return DVH_VAT::TR_KEEP;
	}

	std::vector<double> originXY;
	originXY.push_back(0.0);
	originXY.push_back(0.0);

	if (actuator->Move(originXY, DVH_VAT::Narrow) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveToOrigin failed.");

	EnterStateWithTimeout(MoveTargetPositionXY, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1MoveToStartPositionTask::HandleMoveTargetPositionXY(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	std::vector<double> originXY;
	originXY.push_back(0.0);
	originXY.push_back(0.0);

	if (actuator->isMove(originXY, DVH_VAT::Narrow) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "MoveToStartPosition: SafeZ timeout.");

		return DVH_VAT::TR_KEEP;
	}

	if (actuator->Move(m_targetPosition, DVH_VAT::Narrow) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveToFocusInspPos (XY) failed.");

	EnterStateWithTimeout(MoveTargetPositionZ, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1MoveToStartPositionTask::HandleMoveTargetPositionZ(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	if (actuator->isMove(m_targetPosition, DVH_VAT::Narrow) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "MoveToStartPosition: Move timeout.");

		return DVH_VAT::TR_KEEP;
	}

	if (actuator->MoveZ(m_targetPosition[2]) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveToFocusInspPos (Z) failed.");

	EnterStateWithTimeout(CompleteMove, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1MoveToStartPositionTask::HandleCompleteMove(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "MoveToStartPosition: actuator is null.");

	if (actuator->isMoveZ(m_targetPosition[2]) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "MoveToStartPosition: MoveZ timeout.");

		return DVH_VAT::TR_KEEP;
	}

	EnterState(MoveSafeZ);
	return DVH_VAT::TR_NEXT;
}

