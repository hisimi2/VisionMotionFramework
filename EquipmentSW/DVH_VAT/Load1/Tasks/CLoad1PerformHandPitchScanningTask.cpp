#include "StdAfx.h"
#include "CLoad1PerformHandPitchScanningTask.h"
#include "DVH_VAT/DefineVAT.h"
#include "VisionMemoryKeys.h"

using namespace VAT_LOAD1::Task;

CLoad1PerformHandPitchScanningTask::CLoad1PerformHandPitchScanningTask()
	: m_currentScanIndex(0)
	, m_centerPositionX(0.0)
	, m_centerPositionY(0.0)
	, m_focusPositionZ(0.0)
	, m_safePositionZ(0.0)
	, m_cameraId(0)
	, m_currentTargetPositionX(0.0)
	, m_currentTargetPositionY(0.0)
	, m_currentPitchMode(DVH_VAT::Narrow)
	, m_pickerMaxRow(0)
	, m_pickerMaxCol(0)
	, m_visionTimeoutMs(30000)
	, m_moveTimeoutMs(10000)
	, m_standardPickerCol(0.0)
	, m_standardPickerRow(0.0)
	, m_pickerGapX_Narrow(0.0)
	, m_pickerGapX_Wide(0.0)
	, m_pickerGapY_Narrow(0.0)
	, m_pickerGapY_Wide(0.0)
	, m_isScanPointsBuilt(false)
{
}

CLoad1PerformHandPitchScanningTask::~CLoad1PerformHandPitchScanningTask()
{
}

void CLoad1PerformHandPitchScanningTask::OnInitialize(DVH_VAT::VAT_Context& ctx)
{
	m_safePositionZ = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_SAFE_Z, 0.0);
	m_pickerMaxRow = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_PICKER_MAX_ROW, 0);
	m_pickerMaxCol = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_PICKER_MAX_COL, 0);

	if (m_pickerMaxRow <= 0)
	{
		m_pickerMaxRow = 1;
	}

	if (m_pickerMaxCol <= 0)
	{
		m_pickerMaxCol = 1;
	}

	m_pickerGapX_Narrow = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_PICKER_GAP_X_NARROW, 0.0);
	m_pickerGapX_Wide = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_PICKER_GAP_X_WIDE, m_pickerGapX_Narrow);
	m_pickerGapY_Narrow = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_PICKER_GAP_Y_NARROW, 0.0);
	m_pickerGapY_Wide = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_PICKER_GAP_Y_WIDE, m_pickerGapY_Narrow);
	m_standardPickerCol = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_STANDARD_PICKER_COL, 0.0);
	m_standardPickerRow = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_STANDARD_PICKER_ROW, 0.0);

	const int moveTimeoutMs = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_MOTION_TIMEOUT_MS, m_moveTimeoutMs);
	if (moveTimeoutMs > 0)
		m_moveTimeoutMs = moveTimeoutMs;

	m_cameraId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_CAM_INDEX, 0);

	m_scanPoints.clear();
	m_currentScanIndex = 0;
	m_currentPitchMode = DVH_VAT::Narrow;
	m_isScanPointsBuilt = false;
	m_currentTargetPositionX = 0.0;
	m_currentTargetPositionY = 0.0;

	EnterState(MoveSafeZ);
}

void CLoad1PerformHandPitchScanningTask::BuildScanPoints(
	double standardPickerCol,
	double standardPickerRow,
	double pickerGapX_Narrow,
	double pickerGapX_Wide,
	double pickerGapY_Narrow,
	double pickerGapY_Wide)
{
	m_scanPoints.clear();

	for (int pitchMode = DVH_VAT::Narrow; pitchMode <= DVH_VAT::Wide; ++pitchMode)
	{
		const double pickerGapX = (pitchMode == DVH_VAT::Wide) ? pickerGapX_Wide : pickerGapX_Narrow;
		const double pickerGapY = (pitchMode == DVH_VAT::Wide) ? pickerGapY_Wide : pickerGapY_Narrow;

		for (int row = 0; row < m_pickerMaxRow; ++row)
		{
			const int firstCol = 0;
			HandPitchScanPoint firstPoint;
			firstPoint.row = row;
			firstPoint.col = firstCol;
			firstPoint.pitchMode = static_cast<DVH_VAT::PitchStatus>(pitchMode);
			firstPoint.targetPositionX = m_centerPositionX + (standardPickerCol - firstCol) * pickerGapX;
			firstPoint.targetPositionY = m_centerPositionY - (standardPickerRow - row) * pickerGapY;
			m_scanPoints.push_back(firstPoint);
		}

		for (int row = 0; row < m_pickerMaxRow; ++row)
		{
			const int lastCol = m_pickerMaxCol - 1;
			HandPitchScanPoint lastPoint;
			lastPoint.row = row;
			lastPoint.col = lastCol;
			lastPoint.pitchMode = static_cast<DVH_VAT::PitchStatus>(pitchMode);
			lastPoint.targetPositionX = m_centerPositionX + (standardPickerCol - lastCol) * pickerGapX;
			lastPoint.targetPositionY = m_centerPositionY - (standardPickerRow - row) * pickerGapY;
			m_scanPoints.push_back(lastPoint);
		}
	}

	m_isScanPointsBuilt = true;
}

DVH_VAT::TaskResult CLoad1PerformHandPitchScanningTask::OnPoll(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	switch (GetState())
	{
	case MoveSafeZ:           return HandleMoveSafeZ(ctx, actuator);
	case MoveOrigin:          return HandleMoveOrigin(ctx, actuator);
	case MoveHandPitch:       return HandleMoveHandPitch(ctx, actuator);
	case MoveFocusPositionZ:  return HandleMoveFocusPositionZ(ctx, actuator);
	case VisionRequest:       return HandleVisionRequest(ctx, actuator);
	case VisionWait:          return HandleVisionWait(ctx, actuator);
	case ReturnHome:          return HandleReturnHome(ctx, actuator);
	case SaveHandPitchResult: return HandleSaveHandPitchResult(ctx);
	default:                  return DVH_VAT::TR_ERROR;
	}
}

DVH_VAT::TaskResult CLoad1PerformHandPitchScanningTask::HandleMoveSafeZ(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Calibration: actuator is null.");

	if (actuator->MoveZ(0.0) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "Calibration: MoveToSafeZ failed.");

	std::vector<double> currentPosition = actuator->getPosition();
	m_centerPositionX = currentPosition[0];
	m_centerPositionY = currentPosition[1];
	m_focusPositionZ = currentPosition[2];

	if (!m_isScanPointsBuilt)
	{
		BuildScanPoints(
			m_standardPickerCol,
			m_standardPickerRow,
			m_pickerGapX_Narrow,
			m_pickerGapX_Wide,
			m_pickerGapY_Narrow,
			m_pickerGapY_Wide);
	}

	EnterStateWithTimeout(MoveOrigin, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformHandPitchScanningTask::HandleMoveOrigin(
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

	if (actuator->Move(originXY, m_currentPitchMode) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "HandPitch: MoveToOrigin failed.");

	m_currentTargetPositionX = 0.0;
	m_currentTargetPositionY = 0.0;

	EnterStateWithTimeout(MoveHandPitch, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformHandPitchScanningTask::HandleMoveHandPitch(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "XY Move Fail: actuator null");

	std::vector<double> currentXY;
	currentXY.push_back(m_currentTargetPositionX);
	currentXY.push_back(m_currentTargetPositionY);

	if (actuator->isMove(currentXY, m_currentPitchMode) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Calibration: Origin XY timeout.");

		return DVH_VAT::TR_KEEP;
	}

	if (m_currentScanIndex >= m_scanPoints.size())
	{
		EnterStateWithTimeout(ReturnHome, m_moveTimeoutMs);
		return DVH_VAT::TR_KEEP;
	}

	const HandPitchScanPoint& scanPoint = m_scanPoints[m_currentScanIndex];

	m_currentTargetPositionX = scanPoint.targetPositionX;
	m_currentTargetPositionY = scanPoint.targetPositionY;
	m_currentPitchMode = scanPoint.pitchMode;

	std::vector<double> targetXY;
	targetXY.push_back(m_currentTargetPositionX);
	targetXY.push_back(m_currentTargetPositionY);

	if (actuator->Move(targetXY, m_currentPitchMode) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "XY Move Fail");

	EnterStateWithTimeout(MoveFocusPositionZ, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformHandPitchScanningTask::HandleMoveFocusPositionZ(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "HandPitch: actuator is null.");

	std::vector<double> targetXY;
	targetXY.push_back(m_currentTargetPositionX);
	targetXY.push_back(m_currentTargetPositionY);

	if (actuator->isMove(targetXY, m_currentPitchMode) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Z Down Fail");

		return DVH_VAT::TR_KEEP;
	}

	if (actuator->MoveZ(m_focusPositionZ) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "HandPitch: MoveToFocusZ failed.");

	EnterState(VisionRequest);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformHandPitchScanningTask::HandleVisionRequest(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Vision request: actuator null");

	if (actuator->isMoveZ(m_focusPositionZ) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "HandPitch: MoveZ timeout.");

		return DVH_VAT::TR_KEEP;
	}

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

DVH_VAT::TaskResult CLoad1PerformHandPitchScanningTask::HandleVisionWait(
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

	HandPitchScanPoint& scanPoint = m_scanPoints[m_currentScanIndex];
	scanPoint.measuredOffsetX = offsetX;
	scanPoint.measuredOffsetY = offsetY;

	if (actuator)
	{
		actuator->SetLightState(m_cameraId, false);
	}

	++m_currentScanIndex;

	if (m_currentScanIndex >= m_scanPoints.size())
	{
		EnterState(ReturnHome);
		return DVH_VAT::TR_KEEP;
	}

	const HandPitchScanPoint& nextScanPoint = m_scanPoints[m_currentScanIndex];

	if (m_currentTargetPositionX > nextScanPoint.targetPositionX ||
		m_currentTargetPositionY > nextScanPoint.targetPositionY)
	{
		EnterState(MoveSafeZ);
	}
	else
	{
		EnterState(MoveHandPitch);
	}

	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformHandPitchScanningTask::HandleReturnHome(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "ReturnHome: actuator null");

	if (actuator->MoveZ(m_safePositionZ) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "Z Home Return Fail");

	EnterStateWithTimeout(SaveHandPitchResult, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformHandPitchScanningTask::HandleSaveHandPitchResult(
	DVH_VAT::VAT_Context& ctx)
{
	auto repo = ctx.getRepository();
	if (!repo)
	{
		EnterState(CS_ERROR);
		return DVH_VAT::TR_ERROR;
	}

	const int handId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_HAND_ID, 0);
	const int packageId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_PACKAGE_ID, 0);

	for (int row = 0; row < m_pickerMaxRow; ++row)
	{
		const int cols[2] = { 0, m_pickerMaxCol - 1 };

		for (int i = 0; i < 2; ++i)
		{
			const int col = cols[i];

			double narrowOffsetX = 0.0;
			double narrowOffsetY = 0.0;
			double wideOffsetX = 0.0;
			double wideOffsetY = 0.0;

			for (size_t scanIndex = 0; scanIndex < m_scanPoints.size(); ++scanIndex)
			{
				const HandPitchScanPoint& scanPoint = m_scanPoints[scanIndex];

				if (scanPoint.row == row && scanPoint.col == col)
				{
					if (scanPoint.pitchMode == DVH_VAT::Narrow)
					{
						narrowOffsetX = scanPoint.measuredOffsetX;
						narrowOffsetY = scanPoint.measuredOffsetY;
					}
					else if (scanPoint.pitchMode == DVH_VAT::Wide)
					{
						wideOffsetX = scanPoint.measuredOffsetX;
						wideOffsetY = scanPoint.measuredOffsetY;
					}
				}
			}

			repo->SaveHandPitchResult(
				handId,
				packageId,
				col,
				row,
				narrowOffsetX,
				narrowOffsetY,
				wideOffsetX,
				wideOffsetY);
		}
	}

	return DVH_VAT::TR_NEXT;
}

