#include "pch.h"
#include "CLoad1VATPerformCalibrationTask.h"
#include "DefineVAT.h"
#include "VisionMemoryKeys.h"

using namespace VAT_LOAD1::Task;

CLoad1VATPerformCalibrationTask::CLoad1VATPerformCalibrationTask()
	: m_moveTimeoutMs(7000), m_visionTimeoutMs(10000)
	, m_inspectionCount(0), m_maxInspectionCount(0)
	, m_currentPitchMode(VMF::Narrow), m_isWideCheck(false)
	, m_cameraId(0), m_locationId(0), m_packageId(0)
{}

CLoad1VATPerformCalibrationTask::~CLoad1VATPerformCalibrationTask() {}

void CLoad1VATPerformCalibrationTask::OnInitialize(VMF::Context& ctx)
{
	VMF::VisionPosition position;
	if (ctx.PeekVisionPosition(position))
	{
		m_targetPosition = position.pos;
		m_locationId = position.locateId;
	}
	else
	{
		EnterCommonState(CS_ERROR);
		return;
	}

	m_maxInspectionCount = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_MAX_INSP_COUNT, 1);
	if (m_maxInspectionCount <= 0) m_maxInspectionCount = 1;

	const int moveTimeoutMs = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_MOTION_TIMEOUT_MS, m_moveTimeoutMs);
	if (moveTimeoutMs > 0) m_moveTimeoutMs = moveTimeoutMs;

	m_isWideCheck = !ctx.GetSeqParamAs<std::string>(VAT_SEQ_PARAM_WIDE_CHECK, std::string()).empty();
	m_cameraId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_CAMERA_INDEX, 0);
	m_packageId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_PACKAGE_ID, 0);

	m_inspectionCount = 0;
	m_currentPitchMode = VMF::Narrow;
	EnterState(VisionRequest);
}

VMF::TaskResult CLoad1VATPerformCalibrationTask::OnPoll(VMF::Context& ctx, VMF::IActuator* actuator)
{
	switch (GetState())
	{
	case MoveSafeZ:                 return HandleMoveSafeZ(ctx, actuator);
	case MoveOrigin:                return HandleMoveOrigin(ctx, actuator);
	case MoveCalibrationPositionXY: return HandleMoveCalibrationPositionXY(ctx, actuator);
	case MoveFocusPositionZ:        return HandleMoveFocusPositionZ(ctx, actuator);
	case VisionRequest:             return HandleVisionRequest(ctx, actuator);
	case VisionWait:                return HandleVisionWait(ctx, actuator);
	case SaveCalibrationResult:     return HandleSaveCalibrationResult(ctx);
	default:                        return VMF::TR_ERROR;
	}
}

VMF::TaskResult CLoad1VATPerformCalibrationTask::HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "Calibration: actuator is null.");
	if (actuator->MoveZ(0.0) != VMF::ActOk) return SetErrorAndReturn(ctx, "Calibration: MoveToSafeZ failed.");
	EnterStateWithTimeout(MoveOrigin, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformCalibrationTask::HandleMoveOrigin(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "Calibration: actuator is null.");

	if (actuator->isMoveZ(0.0) != VMF::ActOk)
	{
		if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "Calibration: SafeZ timeout.");
		return VMF::TR_KEEP;
	}

	VMF::MotionCommand cmd;
	cmd.Set("X", 0.0);
	cmd.Set("Y", 0.0);
	cmd.pitch = m_currentPitchMode;

	if (actuator->Move(cmd) != VMF::ActOk) return SetErrorAndReturn(ctx, "Calibration: MoveToOrigin failed.");
	EnterStateWithTimeout(MoveCalibrationPositionXY, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformCalibrationTask::HandleMoveCalibrationPositionXY(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "Calibration: actuator is null.");

	VMF::MotionCommand originCmd;
	originCmd.Set("X", 0.0);
	originCmd.Set("Y", 0.0);
	originCmd.pitch = m_currentPitchMode;

	if (actuator->isMove(originCmd) != VMF::ActOk)
	{
		if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "Calibration: Origin XY timeout.");
		return VMF::TR_KEEP;
	}

	VMF::MotionCommand cmd;
	cmd.Set("X", m_targetPosition[0]);
	cmd.Set("Y", m_targetPosition[1]);
	cmd.pitch = m_currentPitchMode;

	if (actuator->Move(cmd) != VMF::ActOk) return SetErrorAndReturn(ctx, "Calibration: MoveToCalibrationPos failed.");
	EnterStateWithTimeout(MoveFocusPositionZ, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformCalibrationTask::HandleMoveFocusPositionZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "Calibration: actuator is null.");

	VMF::MotionCommand cmd;
	cmd.Set("X", m_targetPosition[0]);
	cmd.Set("Y", m_targetPosition[1]);
	cmd.pitch = m_currentPitchMode;

	if (actuator->isMove(cmd) != VMF::ActOk)
	{
		if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "Calibration: FocusPos timeout.");
		return VMF::TR_KEEP;
	}

	if (actuator->MoveZ(m_targetPosition[2]) != VMF::ActOk) return SetErrorAndReturn(ctx, "Calibration: MoveToFocusZ failed.");
	EnterState(VisionRequest);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformCalibrationTask::HandleVisionRequest(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "Calibration: actuator is null.");

	if (actuator->isMoveZ(m_targetPosition[2]) != VMF::ActOk)
	{
		if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "Calibration: MoveZ timeout.");
		return VMF::TR_KEEP;
	}

	actuator->SetLightState(m_cameraId, true);

	auto visionProcessor = ctx.GetVisionProcessorInterface();

	if (m_locationId == 13) ctx.SetSeqParam(VAT_SEQ_PARAM_MOVE_PART, 4);
	else if (m_locationId == 5) ctx.SetSeqParam(VAT_SEQ_PARAM_MOVE_PART, 1);
	else if (m_locationId == 1) ctx.SetSeqParam(VAT_SEQ_PARAM_MOVE_PART, 5);
	else if (m_locationId == 2) ctx.SetSeqParam(VAT_SEQ_PARAM_MOVE_PART, 6);
	else if (m_locationId == 3) ctx.SetSeqParam(VAT_SEQ_PARAM_MOVE_PART, 0);
	else if (m_locationId == 12) ctx.SetSeqParam(VAT_SEQ_PARAM_MOVE_PART, 3);

	visionProcessor->InitializeRecvThread();
	if (!ctx.ExecuteVisionCommand(VMF::Measure)) return SetErrorAndReturn(ctx, "Vision Command Failed");

	EnterStateWithTimeout(VisionWait, m_visionTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformCalibrationTask::HandleVisionWait(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "Vision Timeout");

	auto visionProcessor = ctx.GetVisionProcessorInterface();
	if (!visionProcessor->IsValid(VMF::Measure)) return VMF::TR_KEEP;

	auto data = visionProcessor->GetLatestData(VMF::Measure);
	double offsetX = 0.0, offsetY = 0.0;

	auto itX = data.find(VAT_VISION_KEY_X_OFFSET);
	if (itX != data.end()) offsetX = std::stod(itX->second);
	auto itY = data.find(VAT_VISION_KEY_Y_OFFSET);
	if (itY != data.end()) offsetY = std::stod(itY->second);

	actuator->SetLightState(m_cameraId, false);

	if (m_cameraId < 6) { m_targetPosition[0] += offsetX; m_targetPosition[1] += offsetY; }
	else { m_targetPosition[0] -= offsetX; m_targetPosition[1] -= offsetY; }

	if (m_inspectionCount < m_maxInspectionCount)
	{
		++m_inspectionCount;
		EnterState(MoveSafeZ);
		return VMF::TR_KEEP;
	}

	m_inspectionCount = 0;
	EnterState(SaveCalibrationResult);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformCalibrationTask::HandleSaveCalibrationResult(VMF::Context& ctx)
{
	auto repo = ctx.GetRepository();
	if (repo)
		repo->SaveCalibrationPosResult(m_cameraId, m_locationId, m_packageId, m_targetPosition[0], m_targetPosition[1]);

	if (m_isWideCheck && m_currentPitchMode == VMF::Narrow)
	{
		m_currentPitchMode = VMF::Wide;
		EnterState(MoveSafeZ);
		return VMF::TR_NEXT;
	}

	m_currentPitchMode = VMF::Narrow;

	VMF::VisionPosition position;
	ctx.PopVisionPosition(position);

	if (ctx.IsVisionPositionEmpty()) return VMF::TR_NEXT;

	ctx.PeekVisionPosition(position);
	m_targetPosition = position.pos;
	m_locationId = position.locateId;

	EnterStateWithTimeout(MoveSafeZ, m_moveTimeoutMs);
	return VMF::TR_NEXT;
}