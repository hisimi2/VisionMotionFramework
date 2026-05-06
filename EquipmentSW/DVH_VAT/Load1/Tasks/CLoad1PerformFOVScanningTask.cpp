#include "StdAfx.h"
#include "CLoad1PerformFOVScanningTask.h"
#include "DVH_VAT/DefineVAT.h"
#include "VisionMemoryKeys.h"

using namespace VAT_LOAD1::Task;

CLoad1PerformFOVScanningTask::CLoad1PerformFOVScanningTask()
	: m_centerPositionX(0.0)
	, m_centerPositionY(0.0)
	, m_focusPositionZ(0.0)
	, m_cameraId(0)
	, m_targetPositionX(0.0)
	, m_targetPositionY(0.0)
	, m_fovInterval(2.0)
	, m_visionTimeoutMs(30000)
	, m_moveTimeoutMs(7000)
	, m_currentScanDirection(CENTER)
{
}

CLoad1PerformFOVScanningTask::~CLoad1PerformFOVScanningTask()
{
}

void CLoad1PerformFOVScanningTask::OnInitialize(DVH_VAT::VAT_Context& ctx)
{
	m_cameraId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_CAMERA_INDEX, 0);

	m_fovInterval = 2.0;
	m_currentScanDirection = CENTER;

	EnterState(MoveSafeZ);
}

DVH_VAT::TaskResult CLoad1PerformFOVScanningTask::OnPoll(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	switch (GetState())
	{
	case MoveSafeZ:          return HandleMoveSafeZ(ctx, actuator);
	case MoveOrigin:         return HandleMoveOrigin(ctx, actuator);
	case MoveScanPosition:   return HandleMoveScanPosition(ctx, actuator);
	case MoveFocusPositionZ: return HandleMoveFocusPositionZ(ctx, actuator);
	case VisionRequest:      return HandleVisionRequest(ctx, actuator);
	case VisionWait:         return HandleVisionWait(ctx, actuator);
	case ReturnHome:         return HandleReturnHome(ctx, actuator);
	default:                 return DVH_VAT::TR_ERROR;
	}
}

DVH_VAT::TaskResult CLoad1PerformFOVScanningTask::HandleMoveSafeZ(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Calibration: actuator is null.");

	if (m_currentScanDirection == CENTER)
	{
		std::vector<double> currentPosition = actuator->getPosition();

		m_targetPositionX = m_centerPositionX = currentPosition[0];
		m_targetPositionY = m_centerPositionY = currentPosition[1];
		m_focusPositionZ = currentPosition[2];

		const int nextDirection = static_cast<int>(m_currentScanDirection) + 1;
		m_currentScanDirection = static_cast<FOVDirection>(nextDirection);
	}

	if (actuator->MoveZ(0.0) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "Calibration: MoveToSafeZ failed.");

	EnterStateWithTimeout(MoveOrigin, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformFOVScanningTask::HandleMoveOrigin(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Calibration: actuator is null.");

	if (actuator->isMoveZ(0.0) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Calibration: SafeZ timeout.");

		return DVH_VAT::TR_KEEP;
	}

	std::vector<double> originXY;
	originXY.push_back(0.0);
	originXY.push_back(0.0);

	if (actuator->Move(originXY, DVH_VAT::Narrow) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "Calibration: MoveToOrigin failed.");

	EnterStateWithTimeout(MoveScanPosition, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformFOVScanningTask::HandleMoveScanPosition(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "FOV: actuator is null.");

	std::vector<double> originXY;
	originXY.push_back(0.0);
	originXY.push_back(0.0);

	if (actuator->isMove(originXY, DVH_VAT::Narrow) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "FOV: Origin XY timeout.");

		return DVH_VAT::TR_KEEP;
	}

	m_targetPositionX = m_centerPositionX;
	m_targetPositionY = m_centerPositionY;

	switch (m_currentScanDirection)
	{
	case REAR:
		m_targetPositionY = m_centerPositionY + m_fovInterval;
		break;

	case FRONT:
		m_targetPositionY = m_centerPositionY - m_fovInterval;
		break;

	case LEFT_SIDE:
		m_targetPositionX = m_centerPositionX - m_fovInterval;
		break;

	case RIGHT_SIDE:
		m_targetPositionX = m_centerPositionX + m_fovInterval;
		break;

	default:
		EnterStateWithTimeout(ReturnHome, m_moveTimeoutMs);
		return DVH_VAT::TR_KEEP;
	}

	std::vector<double> targetXY;
	targetXY.push_back(m_targetPositionX);
	targetXY.push_back(m_targetPositionY);

	if (actuator->Move(targetXY, DVH_VAT::Narrow) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "XY Move Fail");

	EnterStateWithTimeout(MoveFocusPositionZ, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformFOVScanningTask::HandleMoveFocusPositionZ(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "FOV: actuator is null.");

	std::vector<double> targetXY;
	targetXY.push_back(m_targetPositionX);
	targetXY.push_back(m_targetPositionY);

	if (actuator->isMove(targetXY, DVH_VAT::Narrow) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "FOV: FocusPos timeout.");

		return DVH_VAT::TR_KEEP;
	}

	if (actuator->MoveZ(m_focusPositionZ) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "FOV: MoveToFocusZ failed.");

	EnterState(VisionRequest);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformFOVScanningTask::HandleVisionRequest(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "FOV: actuator is null.");

	if (actuator->isMoveZ(m_focusPositionZ) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Z Move Time Out");

		return DVH_VAT::TR_KEEP;
	}

	const int visionRequestId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_VISION_FOV_REQUEST_ID, 7);

	ctx.SetSeqParam(VAT_SEQ_PARAM_STATUS, visionRequestId);

	actuator->SetLightState(m_cameraId, true);

	auto visionProcessor = ctx.GetVisionProcessorInterface();
	if (!visionProcessor)
	{
		return SetErrorAndReturn(ctx, "No Vision Processor");
	}

	visionProcessor->InitializeRecvThread();

	if (!ctx.ExecuteVisionCommand(DVH_VAT::Measure))
	{
		return SetErrorAndReturn(ctx, "Vision Command Failed");
	}

	EnterStateWithTimeout(VisionWait, m_visionTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformFOVScanningTask::HandleVisionWait(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (IsDeadlineExpired())
	{
		return SetErrorAndReturn(ctx, "Vision Timeout");
	}

	auto visionProcessor = ctx.GetVisionProcessorInterface();
	if (!visionProcessor)
	{
		return SetErrorAndReturn(ctx, "No Vision Processor");
	}

	if (!visionProcessor->IsValid(DVH_VAT::Measure))
		return DVH_VAT::TR_KEEP;

	auto data = visionProcessor->GetLatestData(DVH_VAT::Measure);

	double offsetX = 0.0;
	double offsetY = 0.0;

	auto itXOffset = data.find(VAT_VISION_KEY_X_OFFSET);
	if (itXOffset != data.end())
		offsetX = std::stod(itXOffset->second);

	auto itYOffset = data.find(VAT_VISION_KEY_Y_OFFSET);
	if (itYOffset != data.end())
		offsetY = std::stod(itYOffset->second);

	(void)offsetX;
	(void)offsetY;

	if (actuator)
	{
		actuator->SetLightState(m_cameraId, false);
	}

	if (static_cast<int>(m_currentScanDirection) < RIGHT_SIDE)
	{
		const int nextDirection = static_cast<int>(m_currentScanDirection) + 1;
		m_currentScanDirection = static_cast<FOVDirection>(nextDirection);
		EnterState(MoveSafeZ);
		return DVH_VAT::TR_KEEP;
	}

	if (!ctx.IsVisionPositionEmpty())
	{
		DVH_VAT::VisionPosition position;
		ctx.PopVisionPosition(position);

		m_currentScanDirection = CENTER;
		EnterState(VisionRequest);
		return DVH_VAT::TR_PREV;
	}

	EnterState(ReturnHome);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformFOVScanningTask::HandleReturnHome(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Z Home Return Fail");

	if (actuator->MoveZ(0.0) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "Z Home Return Fail");

	return DVH_VAT::TR_NEXT;
}

