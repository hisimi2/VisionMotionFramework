#include "pch.h"
#include "CLoad1VATPerformHandPitchScanningTask.h"
#include "DefineVAT.h"
#include "VisionParamKeysVAT.h"

using namespace VMF;
using namespace VMF_PLUGIN;

CLoad1VATPerformHandPitchScanningTask::CLoad1VATPerformHandPitchScanningTask()
	: m_currentScanIndex(0)
	, m_centerPositionX(0.0), m_centerPositionY(0.0)
	, m_focusPositionZ(0.0), m_safePositionZ(0.0)
	, m_cameraId(0)
	, m_currentTargetPositionX(0.0), m_currentTargetPositionY(0.0)
	, m_currentPitchMode(VMF::Narrow)
	, m_pickerMaxRow(0), m_pickerMaxCol(0)
	, m_visionTimeoutMs(30000), m_moveTimeoutMs(10000)
	, m_standardPickerCol(0.0), m_standardPickerRow(0.0)
	, m_pickerGapX_Narrow(0.0), m_pickerGapX_Wide(0.0)
	, m_pickerGapY_Narrow(0.0), m_pickerGapY_Wide(0.0)
	, m_isScanPointsBuilt(false)
{}

CLoad1VATPerformHandPitchScanningTask::~CLoad1VATPerformHandPitchScanningTask() {}

void CLoad1VATPerformHandPitchScanningTask::OnInitialize(VMF::Context& ctx)
{
	m_safePositionZ = GetTaskSeqParamAs<double>(ctx, VAT_SEQ_PARAM_SAFE_Z);
	m_pickerMaxRow = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_PICKER_MAX_ROW);
	m_pickerMaxCol = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_PICKER_MAX_COL);
	if (m_pickerMaxRow <= 0) m_pickerMaxRow = 1;
	if (m_pickerMaxCol <= 0) m_pickerMaxCol = 1;

	m_pickerGapX_Narrow = GetTaskSeqParamAs<double>(ctx, VAT_SEQ_PARAM_PICKER_GAP_X_NARROW);
	m_pickerGapX_Wide = GetTaskSeqParamAs<double>(ctx, VAT_SEQ_PARAM_PICKER_GAP_X_WIDE);
	m_pickerGapY_Narrow = GetTaskSeqParamAs<double>(ctx, VAT_SEQ_PARAM_PICKER_GAP_Y_NARROW);
	m_pickerGapY_Wide = GetTaskSeqParamAs<double>(ctx, VAT_SEQ_PARAM_PICKER_GAP_Y_WIDE);
	m_standardPickerCol = GetTaskSeqParamAs<double>(ctx, VAT_SEQ_PARAM_STANDARD_PICKER_COL);
	m_standardPickerRow = GetTaskSeqParamAs<double>(ctx, VAT_SEQ_PARAM_STANDARD_PICKER_ROW);

    m_moveTimeoutMs = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_MOTION_TIMEOUT_MS);
	

	m_cameraId = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_CAM_INDEX);

	m_scanPoints.clear();
	m_currentScanIndex = 0;
	m_currentPitchMode = VMF::Narrow;
	m_isScanPointsBuilt = false;
	m_currentTargetPositionX = 0.0;
	m_currentTargetPositionY = 0.0;

	EnterState(MoveSafeZ);
}

void CLoad1VATPerformHandPitchScanningTask::BuildScanPoints(
	double standardPickerCol, double standardPickerRow,
	double pickerGapX_Narrow, double pickerGapX_Wide,
	double pickerGapY_Narrow, double pickerGapY_Wide)
{
	m_scanPoints.clear();

	for (int pitchMode = VMF::Narrow; pitchMode <= VMF::Wide; ++pitchMode)
	{
		const double gapX = (pitchMode == VMF::Wide) ? pickerGapX_Wide : pickerGapX_Narrow;
		const double gapY = (pitchMode == VMF::Wide) ? pickerGapY_Wide : pickerGapY_Narrow;

		for (int row = 0; row < m_pickerMaxRow; ++row)
		{
			for (int colIdx : {0, m_pickerMaxCol - 1})
			{
				HandPitchScanPoint pt;
				pt.row = row;
				pt.col = colIdx;
				pt.pitchMode = static_cast<VMF::PitchStatus>(pitchMode);
				pt.targetPositionX = m_centerPositionX + (standardPickerCol - colIdx) * gapX;
				pt.targetPositionY = m_centerPositionY - (standardPickerRow - row)    * gapY;
				m_scanPoints.push_back(pt);
			}
		}
	}
	m_isScanPointsBuilt = true;
}

VMF::TaskResult CLoad1VATPerformHandPitchScanningTask::OnPoll(VMF::Context& ctx, VMF::IActuator* actuator)
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
	default:                  return VMF::TR_ERROR;
	}
}

VMF::TaskResult CLoad1VATPerformHandPitchScanningTask::HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "Calibration: actuator is null.");
	if (actuator->MoveZ(0.0) != VMF::ActOk) return SetErrorAndReturn(ctx, "Calibration: MoveToSafeZ failed.");

    // std::vector<double> pos = actuator->GetEncoder();
    // m_centerPositionX = pos[0];
    // m_centerPositionY = pos[1];
    // m_focusPositionZ = pos[2];

	if (!m_isScanPointsBuilt)
		BuildScanPoints(m_standardPickerCol, m_standardPickerRow,
			m_pickerGapX_Narrow, m_pickerGapX_Wide, m_pickerGapY_Narrow, m_pickerGapY_Wide);

	EnterStateWithTimeout(MoveOrigin, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformHandPitchScanningTask::HandleMoveOrigin(VMF::Context& ctx, VMF::IActuator* actuator)
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

	if (actuator->Move(cmd) != VMF::ActOk) return SetErrorAndReturn(ctx, "HandPitch: MoveToOrigin failed.");

	m_currentTargetPositionX = 0.0;
	m_currentTargetPositionY = 0.0;

	EnterStateWithTimeout(MoveHandPitch, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformHandPitchScanningTask::HandleMoveHandPitch(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "XY Move Fail: actuator null");

	VMF::MotionCommand checkCmd;
	checkCmd.Set("X", m_currentTargetPositionX);
	checkCmd.Set("Y", m_currentTargetPositionY);
	checkCmd.pitch = m_currentPitchMode;

	if (actuator->isMove(checkCmd) != VMF::ActOk)
	{
		if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "Calibration: Origin XY timeout.");
		return VMF::TR_KEEP;
	}

	if (m_currentScanIndex >= m_scanPoints.size())
	{
		EnterStateWithTimeout(ReturnHome, m_moveTimeoutMs);
		return VMF::TR_KEEP;
	}

	const HandPitchScanPoint& pt = m_scanPoints[m_currentScanIndex];
	m_currentTargetPositionX = pt.targetPositionX;
	m_currentTargetPositionY = pt.targetPositionY;
	m_currentPitchMode = pt.pitchMode;

	VMF::MotionCommand cmd;
	cmd.Set("X", m_currentTargetPositionX);
	cmd.Set("Y", m_currentTargetPositionY);
	cmd.pitch = m_currentPitchMode;

	if (actuator->Move(cmd) != VMF::ActOk) return SetErrorAndReturn(ctx, "XY Move Fail");
	EnterStateWithTimeout(MoveFocusPositionZ, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformHandPitchScanningTask::HandleMoveFocusPositionZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "HandPitch: actuator is null.");

	VMF::MotionCommand cmd;
	cmd.Set("X", m_currentTargetPositionX);
	cmd.Set("Y", m_currentTargetPositionY);
	cmd.pitch = m_currentPitchMode;

	if (actuator->isMove(cmd) != VMF::ActOk)
	{
		if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "Z Down Fail");
		return VMF::TR_KEEP;
	}

	if (actuator->MoveZ(m_focusPositionZ) != VMF::ActOk) return SetErrorAndReturn(ctx, "HandPitch: MoveToFocusZ failed.");
	EnterState(VisionRequest);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformHandPitchScanningTask::HandleVisionRequest(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "Vision request: actuator null");

	if (actuator->isMoveZ(m_focusPositionZ) != VMF::ActOk)
	{
		if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "HandPitch: MoveZ timeout.");
		return VMF::TR_KEEP;
	}

	actuator->SetLightState(m_cameraId, true);

	auto visionProcessor = ctx.GetVisionProcessorInterface();
	if (!visionProcessor) return SetErrorAndReturn(ctx, "No Vision Processor");

	visionProcessor->InitializeRecvThread();
	if (!ctx.ExecuteVisionCommand(VMF::Measure)) return SetErrorAndReturn(ctx, "Vision Command Failed");

	EnterStateWithTimeout(VisionWait, m_visionTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformHandPitchScanningTask::HandleVisionWait(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "Vision Timeout");

	auto visionProcessor = ctx.GetVisionProcessorInterface();
	if (!visionProcessor) return SetErrorAndReturn(ctx, "No Vision Processor");
	if (!visionProcessor->IsValid(VMF::Measure)) return VMF::TR_KEEP;

	auto data = visionProcessor->GetLatestData(VMF::Measure);
	double offsetX = 0.0, offsetY = 0.0;

	//auto itX = data.find(VAT_VISION_KEY_X_OFFSET); if (itX != data.end()) offsetX = std::stod(itX->second);
	//auto itY = data.find(VAT_VISION_KEY_Y_OFFSET); if (itY != data.end()) offsetY = std::stod(itY->second);

	m_scanPoints[m_currentScanIndex].measuredOffsetX = offsetX;
	m_scanPoints[m_currentScanIndex].measuredOffsetY = offsetY;

	if (actuator) actuator->SetLightState(m_cameraId, false);

	++m_currentScanIndex;

	if (m_currentScanIndex >= m_scanPoints.size())
	{
		EnterState(ReturnHome);
		return VMF::TR_KEEP;
	}

	const HandPitchScanPoint& next = m_scanPoints[m_currentScanIndex];
	if (m_currentTargetPositionX > next.targetPositionX ||
		m_currentTargetPositionY > next.targetPositionY)
		EnterState(MoveSafeZ);
	else
		EnterState(MoveHandPitch);

	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformHandPitchScanningTask::HandleReturnHome(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "ReturnHome: actuator null");
	if (actuator->MoveZ(m_safePositionZ) != VMF::ActOk) return SetErrorAndReturn(ctx, "Z Home Return Fail");
	EnterStateWithTimeout(SaveHandPitchResult, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformHandPitchScanningTask::HandleSaveHandPitchResult(VMF::Context& ctx)
{
	auto repo = ctx.GetRepository();
	if (!repo) { EnterState(CS_ERROR); return VMF::TR_ERROR; }

	const int handId = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_HAND_ID);
	const int packageId = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_PACKAGE_ID);

	for (int row = 0; row < m_pickerMaxRow; ++row)
	{
		const int cols[2] = { 0, m_pickerMaxCol - 1 };
		for (int i = 0; i < 2; ++i)
		{
			const int col = cols[i];
			double narrowOffsetX = 0.0, narrowOffsetY = 0.0;
			double wideOffsetX = 0.0, wideOffsetY = 0.0;

			for (const auto& pt : m_scanPoints)
			{
				if (pt.row == row && pt.col == col)
				{
					if (pt.pitchMode == VMF::Narrow) { narrowOffsetX = pt.measuredOffsetX; narrowOffsetY = pt.measuredOffsetY; }
					else { wideOffsetX = pt.measuredOffsetX; wideOffsetY = pt.measuredOffsetY; }
				}
			}
			repo->SaveHandPitchResult(handId, packageId, col, row, narrowOffsetX, narrowOffsetY, wideOffsetX, wideOffsetY);
		}
	}
	return VMF::TR_NEXT;
}
