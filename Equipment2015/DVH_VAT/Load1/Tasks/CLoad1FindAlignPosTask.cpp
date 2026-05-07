#include "StdAfx.h"
#include "CLoad1FindAlignPosTask.h"
#include "DVH_VAT/DefineVAT.h"
#include "VisionMemoryKeys.h"

using namespace VAT_LOAD1::Task;

CLoad1FindAlignPosTask::CLoad1FindAlignPosTask()
	: m_moveTimeoutMs(7000)
	, m_visionTimeoutMs(10000)
	, m_inspectionCount(0)
	, m_maxInspectionCount(0)
	, m_cameraId(0)
	, m_locationId(0)
	, m_packageId(0)
{
}

CLoad1FindAlignPosTask::~CLoad1FindAlignPosTask()
{
}

void CLoad1FindAlignPosTask::OnInitialize(DVH_VAT::VAT_Context& ctx)
{
	DVH_VAT::VisionPosition position;
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

	m_maxInspectionCount = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_MAX_INSP_COUNT, 0);
	if (m_maxInspectionCount < 0)
	{
		m_maxInspectionCount = 0;
	}

	const int moveTimeoutMs = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_MOTION_TIMEOUT_MS, m_moveTimeoutMs);
	if (moveTimeoutMs > 0)
	{
		m_moveTimeoutMs = moveTimeoutMs;
	}

	m_cameraId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_CAMERA_INDEX, 0);
	m_packageId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_PACKAGE_ID, 0);

	m_inspectionCount = 0;
	EnterState(VisionRequest);
}

DVH_VAT::TaskResult CLoad1FindAlignPosTask::OnPoll(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
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
	default:                    return DVH_VAT::TR_ERROR;
	}
}

DVH_VAT::TaskResult CLoad1FindAlignPosTask::HandleMoveSafeZ(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Calibration: actuator is null.");

	if (actuator->MoveZ(0.0) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "Calibration: MoveToSafeZ failed.");

	EnterStateWithTimeout(MoveOrigin, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1FindAlignPosTask::HandleMoveOrigin(
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

	std::vector<double> originPosition;
	originPosition.push_back(0.0);
	originPosition.push_back(0.0);
	originPosition.push_back(0.0);
	originPosition.push_back(0.0);
	originPosition.push_back(0.0);

	if (actuator->Move(originPosition, DVH_VAT::Narrow) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "Calibration: MoveToOrigin failed.");

	EnterStateWithTimeout(MoveCalibrationPosXY, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1FindAlignPosTask::HandleMoveCalibrationXY(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Calibration: actuator is null.");

	std::vector<double> originXY;
	originXY.push_back(0.0);
	originXY.push_back(0.0);

	if (actuator->isMove(originXY, DVH_VAT::Narrow) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Calibration: Origin XY timeout.");

		return DVH_VAT::TR_KEEP;
	}

	if (actuator->Move(m_targetPosition, DVH_VAT::Narrow) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "Calibration: MoveToCalibrationPos failed.");

	EnterStateWithTimeout(MoveFocusPosZ, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1FindAlignPosTask::HandleMoveFocusZ(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Calibration: actuator is null.");

	if (actuator->isMove(m_targetPosition, DVH_VAT::Narrow) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Calibration: FocusPos timeout.");

		return DVH_VAT::TR_KEEP;
	}

	if (actuator->MoveZ(m_targetPosition[2]) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "Calibration: MoveToFocusZ failed.");

	EnterState(VisionRequest);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1FindAlignPosTask::HandleVisionRequest(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (actuator->isMoveZ(m_targetPosition[2]) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Calibration: FocusPosZ timeout.");

		return DVH_VAT::TR_KEEP;
	}

	actuator->SetLightState(m_cameraId, true);

	auto visionProcessor = ctx.GetVisionProcessorInterface();
	visionProcessor->InitializeRecvThread();

	DVH_VAT::VisionPosition position;
	ctx.PeekVisionPosition(position);
	ctx.SetSeqParam(VAT_SEQ_PARAM_INSPECTION_TYPE, position.visionRequestId);

	if (!ctx.ExecuteVisionCommand(DVH_VAT::Measure))
	{
		return SetErrorAndReturn(ctx, "Vision Command Failed");
	}

	EnterStateWithTimeout(VisionWait, m_visionTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1FindAlignPosTask::HandleVisionWait(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (IsDeadlineExpired())
	{
		return SetErrorAndReturn(ctx, "Vision Timeout");
	}

	auto visionProcessor = ctx.GetVisionProcessorInterface();

	if (!visionProcessor->IsValid(DVH_VAT::Measure))
		return DVH_VAT::TR_KEEP;

	auto data = visionProcessor->GetLatestData(DVH_VAT::Measure);

	double offsetX = 0.0;
	double offsetY = 0.0;

	auto itXOffset = data.find(DVH_VAT::X_OFFSET);
	if (itXOffset != data.end())
		offsetX = std::stod(itXOffset->second);

	auto itYOffset = data.find(DVH_VAT::Y_OFFSET);
	if (itYOffset != data.end())
		offsetY = std::stod(itYOffset->second);

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

	actuator->SetLightState(m_cameraId, false);

	if (m_inspectionCount < m_maxInspectionCount)
	{
		++m_inspectionCount;
		EnterState(MoveSafeZ);
		return DVH_VAT::TR_KEEP;
	}

	m_inspectionCount = 0;
	EnterState(SaveCalibrationResult);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1FindAlignPosTask::HandleSaveCalibrationResult(DVH_VAT::VAT_Context& ctx)
{
	auto repo = ctx.getRepository();

	if (repo)
	{
		repo->SaveCalibrationPosResult(
			m_cameraId,
			m_locationId,
			m_packageId,
			m_targetPosition[0],
			m_targetPosition[1]);
	}

	DVH_VAT::VisionPosition position;
	ctx.PopVisionPosition(position);

	position.pos = m_targetPosition;
	m_alignedPositions.push_back(position);

	if (ctx.IsVisionPositionEmpty())
	{
		for (size_t i = 0; i < m_alignedPositions.size(); ++i)
		{
			ctx.AddVisionPosition(m_alignedPositions[i]);
		}

		m_alignedPositions.clear();
		return DVH_VAT::TR_NEXT;
	}

	ctx.PeekVisionPosition(position);
	m_targetPosition = position.pos;
	m_locationId = position.locateId;

	EnterStateWithTimeout(VisionRequest, m_moveTimeoutMs);
	return DVH_VAT::TR_PREV;
}

