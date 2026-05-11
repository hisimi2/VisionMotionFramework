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

void CLoad1PerformFOVScanningTask::OnInitialize(VMF::VAT_Context& ctx)
{
	m_cameraId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_CAMERA_INDEX, 0);

	m_fovInterval = 2.0;
	m_currentScanDirection = CENTER;

	EnterState(MoveSafeZ);
}

VMF::TaskResult CLoad1PerformFOVScanningTask::OnPoll(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
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
	default:                 return VMF::TR_ERROR;
	}
}

VMF::TaskResult CLoad1PerformFOVScanningTask::HandleMoveSafeZ(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
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

	if (actuator->MoveZ(0.0) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "Calibration: MoveToSafeZ failed.");

	EnterStateWithTimeout(MoveOrigin, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformFOVScanningTask::HandleMoveOrigin(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Calibration: actuator is null.");

	if (actuator->isMoveZ(0.0) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Calibration: SafeZ timeout.");

		return VMF::TR_KEEP;
	}

	std::vector<double> originXY;
	originXY.push_back(0.0);
	originXY.push_back(0.0);

	if (actuator->Move(originXY, VMF::Narrow) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "Calibration: MoveToOrigin failed.");

	EnterStateWithTimeout(MoveScanPosition, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformFOVScanningTask::HandleMoveScanPosition(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "FOV: actuator is null.");

	std::vector<double> originXY;
	originXY.push_back(0.0);
	originXY.push_back(0.0);

	if (actuator->isMove(originXY, VMF::Narrow) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "FOV: Origin XY timeout.");

		return VMF::TR_KEEP;
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
		return VMF::TR_KEEP;
	}

	std::vector<double> targetXY;
	targetXY.push_back(m_targetPositionX);
	targetXY.push_back(m_targetPositionY);

	if (actuator->Move(targetXY, VMF::Narrow) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "XY Move Fail");

	EnterStateWithTimeout(MoveFocusPositionZ, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformFOVScanningTask::HandleMoveFocusPositionZ(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "FOV: actuator is null.");

	std::vector<double> targetXY;
	targetXY.push_back(m_targetPositionX);
	targetXY.push_back(m_targetPositionY);

	if (actuator->isMove(targetXY, VMF::Narrow) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "FOV: FocusPos timeout.");

		return VMF::TR_KEEP;
	}

	if (actuator->MoveZ(m_focusPositionZ) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "FOV: MoveToFocusZ failed.");

	EnterState(VisionRequest);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformFOVScanningTask::HandleVisionRequest(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "FOV: actuator is null.");

	if (actuator->isMoveZ(m_focusPositionZ) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Z Move Time Out");

		return VMF::TR_KEEP;
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

	if (!ctx.ExecuteVisionCommand(VMF::Measure))
	{
		return SetErrorAndReturn(ctx, "Vision Command Failed");
	}

	EnterStateWithTimeout(VisionWait, m_visionTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformFOVScanningTask::HandleVisionWait(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
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

	if (!visionProcessor->IsValid(VMF::Measure))
		return VMF::TR_KEEP;

	auto data = visionProcessor->GetLatestData(VMF::Measure);

	double offsetX = 0.0;
	double offsetY = 0.0;

	auto itXOffset = data.find(VMF::X_OFFSET);
	if (itXOffset != data.end())
		offsetX = std::stod(itXOffset->second);

	auto itYOffset = data.find(VMF::Y_OFFSET);
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
		return VMF::TR_KEEP;
	}

	if (!ctx.IsVisionPositionEmpty())
	{
		VMF::VisionPosition position;
		ctx.PopVisionPosition(position);

		m_currentScanDirection = CENTER;
		EnterState(VisionRequest);
		return VMF::TR_PREV;
	}

	EnterState(ReturnHome);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformFOVScanningTask::HandleReturnHome(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Z Home Return Fail");

	if (actuator->MoveZ(0.0) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "Z Home Return Fail");

	return VMF::TR_NEXT;
}

