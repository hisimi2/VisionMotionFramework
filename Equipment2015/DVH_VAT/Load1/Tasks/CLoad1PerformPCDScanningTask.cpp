#include "StdAfx.h"
#include "CLoad1PerformPCDScanningTask.h"
#include "DVH_VAT/DefineVAT.h"
#include "VisionMemoryKeys.h"

using namespace VAT_LOAD1::Task;

CLoad1PerformPCDScanningTask::CLoad1PerformPCDScanningTask()
	: m_scanAxis(Axis_X)
	, m_scanPitch(0.0)
	, m_startAxisPulse(0.0)
	, m_endAxisPulse(0.0)
	, m_targetPositionX(0.0)
	, m_targetPositionY(0.0)
	, m_targetTablePosition(-65.0)
	, m_focusPositionZ(0.0)
	, m_pulseDistanceResult(0.0)
	, m_scaleResult(0.0)
	, m_moveTimeoutMs(7000)
	, m_visionTimeoutMs(30000)
	, m_isStartOffsetMeasured(false)
	, m_inspectionCount(0)
	, m_maxInspectionCount(3)
	, m_cameraId(0)
{
}

CLoad1PerformPCDScanningTask::~CLoad1PerformPCDScanningTask()
{
}

void CLoad1PerformPCDScanningTask::OnInitialize(VMF::VAT_Context& ctx)
{
	m_cameraId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_CAMERA_INDEX, 0);
	m_scanPitch = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_PCD_PITCH, 0.0);
	const std::string scanAxisStr = ctx.GetSeqParamAs<std::string>(VAT_SEQ_PARAM_SCAN_AXIS, std::string());

	if (!scanAxisStr.empty())
	{
		if (scanAxisStr == "X" || scanAxisStr == "x")
			m_scanAxis = Axis_X;
		else
			m_scanAxis = Axis_Y;
	}

    VMF::VisionPosition position;
    ctx.PeekVisionPosition(position);
    m_targetPositionX = position.pos[0];
    m_targetPositionY = position.pos[1];
    m_focusPositionZ = position.pos[2];
    m_targetTablePosition = position.pos[3];

	const int moveTimeoutMs = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_MOTION_TIMEOUT_MS, m_moveTimeoutMs);
	if (moveTimeoutMs > 0)
		m_moveTimeoutMs = moveTimeoutMs;

	const int visionTimeoutMs = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_VISION_TIMEOUT_MS, m_visionTimeoutMs);
	if (visionTimeoutMs > 0)
		m_visionTimeoutMs = visionTimeoutMs;

	m_isStartOffsetMeasured = false;
	m_inspectionCount = 0;
	m_maxInspectionCount = 3;

	const int maxInspectionCount = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_MAX_INSP_COUNT, m_maxInspectionCount);
	if (maxInspectionCount > 0)
		m_maxInspectionCount = maxInspectionCount;

	EnterState(VisionRequest);
}

VMF::TaskResult CLoad1PerformPCDScanningTask::OnPoll(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	switch (GetState())
	{
	case MoveSafeZ:             return HandleMoveSafeZ(ctx, actuator);
	case MoveOrigin:            return HandleMoveOrigin(ctx, actuator);
	case MoveTargetPositionXY:  return HandleMoveTargetPositionXY(ctx, actuator);
	case MoveFocusPositionZ:    return HandleMoveFocusPositionZ(ctx, actuator);
	case VisionRequest:         return HandleVisionRequest(ctx, actuator);
	case VisionWait:            return HandleVisionWait(ctx, actuator);
	case CalculatePCD:          return HandleCalculatePCD(ctx, actuator);
	case SavePCDResult:         return HandleSavePCDResult(ctx);
	default:                    return VMF::TR_ERROR;
	}
}

VMF::TaskResult CLoad1PerformPCDScanningTask::HandleMoveSafeZ(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "PerformPCDScanning: actuator is null.");

	if (actuator->MoveZ(0.0) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "PerformPCDScanning: MoveToSafeZ failed.");

	EnterStateWithTimeout(MoveOrigin, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformPCDScanningTask::HandleMoveOrigin(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "PerformCalibration: actuator is null.");

	if (actuator->isMoveZ(0.0) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "PerformCalibration: SafeZ timeout.");

		return VMF::TR_KEEP;
	}

	std::vector<double> originPosition;
	originPosition.push_back(0.0);
	originPosition.push_back(0.0);
	originPosition.push_back(m_focusPositionZ);
	originPosition.push_back(m_targetTablePosition);
	originPosition.push_back(m_targetTablePosition);

	if (actuator->Move(originPosition, VMF::Narrow) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "PerformCalibration: MoveToFocusInspPos failed.");

	EnterStateWithTimeout(MoveTargetPositionXY, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformPCDScanningTask::HandleMoveTargetPositionXY(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "PerformPCDScanning: actuator is null.");

	std::vector<double> originPosition;
	originPosition.push_back(0.0);
	originPosition.push_back(0.0);
	originPosition.push_back(m_focusPositionZ);
	originPosition.push_back(m_targetTablePosition);
	originPosition.push_back(m_targetTablePosition);

	if (actuator->isMove(originPosition, VMF::Narrow) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "PerformPCDScanning: MoveToSafeZ failed.");

		return VMF::TR_KEEP;
	}

	std::vector<double> targetPosition;
	targetPosition.push_back(m_targetPositionX);
	targetPosition.push_back(m_targetPositionY);
	targetPosition.push_back(m_focusPositionZ);
	targetPosition.push_back(m_targetTablePosition);
	targetPosition.push_back(m_targetTablePosition);

	if (actuator->Move(targetPosition, VMF::Narrow) != VMF::ActOk)
	{
		return SetErrorAndReturn(ctx, "PerformPCDScanning: MoveToFocusInspPos failed.");
	}

	EnterState(MoveFocusPositionZ);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformPCDScanningTask::HandleMoveFocusPositionZ(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "PerformCalibration: actuator is null.");

	std::vector<double> targetPosition;
	targetPosition.push_back(m_targetPositionX);
	targetPosition.push_back(m_targetPositionY);
	targetPosition.push_back(m_focusPositionZ);
	targetPosition.push_back(m_targetTablePosition);
	targetPosition.push_back(m_targetTablePosition);

	if (actuator->isMove(targetPosition, VMF::Narrow) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "PerformCalibration: FocusPos timeout.");

		return VMF::TR_KEEP;
	}

	if (actuator->MoveZ(m_focusPositionZ) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "PerformCalibration: MoveToFocusInspPos failed.");

	EnterState(VisionRequest);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformPCDScanningTask::HandleVisionRequest(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "PerformPCDScanning: light null");

	if (actuator->isMoveZ(m_focusPositionZ) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "PerformPCDScanning: Z Down Fail");

		return VMF::TR_KEEP;
	}

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

VMF::TaskResult CLoad1PerformPCDScanningTask::HandleVisionWait(
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

	if (actuator)
	{
		actuator->SetLightState(m_cameraId, false);
	}

	if (m_inspectionCount < m_maxInspectionCount)
	{
		if (m_cameraId < 6)
		{
			m_targetPositionX += offsetX;
			m_targetPositionY += offsetY;
		}
		else
		{
			m_targetPositionX -= offsetX;
			m_targetPositionY -= offsetY;
		}

		++m_inspectionCount;
		EnterState(MoveSafeZ);
		return VMF::TR_KEEP;
	}

	std::vector<double> axisPulse = actuator->getPulse();

	if (m_isStartOffsetMeasured)
	{
		switch (m_scanAxis)
		{
		case Axis_X:
			m_endAxisPulse = axisPulse[0];
			break;

		case Axis_Y:
			m_endAxisPulse = axisPulse[1];
			break;
		}

		EnterState(CalculatePCD);
		return VMF::TR_KEEP;
	}

	switch (m_scanAxis)
	{
	case Axis_X:
		m_targetPositionX += m_scanPitch;
		m_startAxisPulse = axisPulse[0];
		break;

	case Axis_Y:
		m_targetPositionY += m_scanPitch;
		m_startAxisPulse = axisPulse[1];
		break;
	}

	VMF::VisionPosition position;
	ctx.PopVisionPosition(position);
	position.pos[0] = m_targetPositionX;
	position.pos[1] = m_targetPositionY;
	ctx.AddVisionPosition(position);

	m_isStartOffsetMeasured = true;
	m_inspectionCount = 0;

	EnterState(VisionRequest);
	return VMF::TR_PREV;
}

VMF::TaskResult CLoad1PerformPCDScanningTask::HandleCalculatePCD(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	(void)ctx;
	(void)actuator;

	if (m_startAxisPulse == m_endAxisPulse)
	{
		m_pulseDistanceResult = 0.0;
	}
	else
	{
		m_pulseDistanceResult = m_scanPitch * 2500.0 / (m_startAxisPulse - m_endAxisPulse);
	}

	m_scaleResult = m_pulseDistanceResult * 1000.0 / 2500.0;

	EnterState(SavePCDResult);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformPCDScanningTask::HandleSavePCDResult(
	VMF::VAT_Context& ctx)
{
	(void)ctx;
	return VMF::TR_NEXT;
}
