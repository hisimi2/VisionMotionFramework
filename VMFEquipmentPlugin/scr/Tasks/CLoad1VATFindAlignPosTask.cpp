#include "pch.h"
#include "CLoad1VATFindAlignPosTask.h"
#include "DefineVAT.h"
#include "scr\Protocol\VisionMemoryKeys.h"

using namespace VMF_PLUGIN;

CLoad1VATFindAlignPosTask::CLoad1VATFindAlignPosTask()
	: m_moveTimeoutMs(7000)
	, m_visionTimeoutMs(10000)
	, m_inspectionCount(0)
	, m_maxInspectionCount(0)
	, m_cameraId(0)
	, m_locationId(0)
	, m_packageId(0)
{
}

CLoad1VATFindAlignPosTask::~CLoad1VATFindAlignPosTask()
{
}

void CLoad1VATFindAlignPosTask::OnInitialize(VMF::Context& ctx)
{
    /*
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
    */

	m_maxInspectionCount = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_MAX_INSP_COUNT, 0);
	if (m_maxInspectionCount < 0)
		m_maxInspectionCount = 0;

	const int moveTimeoutMs = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_MOTION_TIMEOUT_MS, m_moveTimeoutMs);
	if (moveTimeoutMs > 0)
		m_moveTimeoutMs = moveTimeoutMs;

	m_cameraId = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_CAMERA_INDEX, 0);
	m_packageId = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_PACKAGE_ID, 0);

	m_inspectionCount = 0;
	EnterState(VisionRequest);
}

VMF::TaskResult CLoad1VATFindAlignPosTask::OnPoll(VMF::Context& ctx, VMF::IActuator* actuator)
{
	switch (GetState())
	{
	case MoveSafeZ:             return HandleMoveSafeZ(ctx, actuator);
	case MoveOrigin:            return HandleMoveOrigin(ctx, actuator);
	case MoveCalibrationPosXY:  return HandleMoveCalibrationXY(ctx, actuator);
	case MoveFocusPosZ:         return HandleMoveFocusZ(ctx, actuator);
	case VisionRequest:         return HandleVisionRequest(ctx, actuator);
	case VisionWait:            return HandleVisionWait(ctx, actuator);
	case SaveCalibrationResult: return HandleSaveCalibrationResult(ctx);
	default:                    return VMF::TR_ERROR;
	}
}

VMF::TaskResult CLoad1VATFindAlignPosTask::HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Calibration: actuator is null.");

	if (actuator->MoveZ(0.0) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "Calibration: MoveToSafeZ failed.");

	EnterStateWithTimeout(MoveOrigin, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATFindAlignPosTask::HandleMoveOrigin(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Calibration: actuator is null.");

	if (actuator->isMoveZ(0.0) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Calibration: SafeZ timeout.");
		return VMF::TR_KEEP;
	}

	VMF::MotionCommand cmd;
	cmd.Set("X", 0.0);
	cmd.Set("Y", 0.0);
	cmd.Set("T1", 0.0);
	cmd.Set("T2", 0.0);
	cmd.pitch = VMF::Narrow;

	if (actuator->Move(cmd) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "Calibration: MoveToOrigin failed.");

	EnterStateWithTimeout(MoveCalibrationPosXY, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATFindAlignPosTask::HandleMoveCalibrationXY(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Calibration: actuator is null.");

	VMF::MotionCommand originCmd;
	originCmd.Set("X", 0.0);
	originCmd.Set("Y", 0.0);
	originCmd.pitch = VMF::Narrow;

	if (actuator->isMove(originCmd) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Calibration: Origin XY timeout.");
		return VMF::TR_KEEP;
	}

	VMF::MotionCommand cmd;
	cmd.Set("X", m_targetPosition[0]);
	cmd.Set("Y", m_targetPosition[1]);
	cmd.pitch = VMF::Narrow;

	if (actuator->Move(cmd) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "Calibration: MoveToCalibrationPos failed.");

	EnterStateWithTimeout(MoveFocusPosZ, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATFindAlignPosTask::HandleMoveFocusZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Calibration: actuator is null.");

	VMF::MotionCommand cmd;
	cmd.Set("X", m_targetPosition[0]);
	cmd.Set("Y", m_targetPosition[1]);
	cmd.pitch = VMF::Narrow;

	if (actuator->isMove(cmd) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Calibration: FocusPos timeout.");
		return VMF::TR_KEEP;
	}

	if (actuator->MoveZ(m_targetPosition[2]) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "Calibration: MoveToFocusZ failed.");

	EnterState(VisionRequest);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATFindAlignPosTask::HandleVisionRequest(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Calibration: actuator is null.");

	if (actuator->isMoveZ(m_targetPosition[2]) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Calibration: FocusPosZ timeout.");
		return VMF::TR_KEEP;
	}

	actuator->SetLightState(m_cameraId, true);

	auto visionProcessor = ctx.GetVisionProcessorInterface();
	visionProcessor->InitializeRecvThread();


    /*
	VMF::VisionPosition position;
	ctx.PeekVisionPosition(position);
	ctx.SetSeqParam(VAT_SEQ_PARAM_INSPECTION_TYPE, position.visionRequestId);
    */

	if (!ctx.ExecuteVisionCommand(VMF::Measure))
		return SetErrorAndReturn(ctx, "Vision Command Failed");

	EnterStateWithTimeout(VisionWait, m_visionTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATFindAlignPosTask::HandleVisionWait(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (IsDeadlineExpired())
		return SetErrorAndReturn(ctx, "Vision Timeout");

	auto visionProcessor = ctx.GetVisionProcessorInterface();
	if (!visionProcessor->IsValid(VMF::Measure))
		return VMF::TR_KEEP;

	auto data = visionProcessor->GetLatestData(VMF::Measure);

	double offsetX = 0.0;
	double offsetY = 0.0;

    /*
	auto itX = data.find(VAT_VISION_KEY_X_OFFSET);
	if (itX != data.end()) offsetX = std::stod(itX->second);
	auto itY = data.find(VAT_VISION_KEY_Y_OFFSET);
	if (itY != data.end()) offsetY = std::stod(itY->second);

	if (m_cameraId < 6)
	{
		m_targetPosition[0] += offsetX;
		m_targetPosition[1] += offsetY;
	}
	else
	{
		m_targetPosition[0] -= offsetX;
		m_targetPosition[1] -= offsetY;
	}
    */

	actuator->SetLightState(m_cameraId, false);

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

VMF::TaskResult CLoad1VATFindAlignPosTask::HandleSaveCalibrationResult(VMF::Context& ctx)
{
	auto repo = ctx.GetRepository();
	if (repo)
	{
		repo->SaveCalibrationPosResult(
			m_cameraId,
			m_locationId,
			m_packageId,
			m_targetPosition[0],
			m_targetPosition[1]);
	}

    /*
	VMF::VisionPosition position;
	ctx.PopVisionPosition(position);

	position.pos = m_targetPosition;
	m_alignedPositions.push_back(position);

	if (ctx.IsVisionPositionEmpty())
	{
		for (size_t i = 0; i < m_alignedPositions.size(); ++i)
			ctx.AddVisionPosition(m_alignedPositions[i]);

		m_alignedPositions.clear();
		return VMF::TR_NEXT;
	}

	ctx.PeekVisionPosition(position);
	m_targetPosition = position.pos;
	m_locationId = position.locateId;
    */

	EnterStateWithTimeout(VisionRequest, m_moveTimeoutMs);
	return VMF::TR_PREV;
}
