#include "pch.h"
#include "CLoad1VATPerformPickerFOVScanningTask.h"
#include "DefineVAT.h"

using namespace VMF;
using namespace VMF_PLUGIN;

CLoad1VATPerformPickerFOVScanningTask::CLoad1VATPerformPickerFOVScanningTask()
	: m_currentScanDirection(CENTER)
	, m_centerPositionX(0.0), m_centerPositionY(0.0), m_focusPositionZ(0.0)
	, m_fovIntervalX(0.0), m_fovIntervalY(0.0)
	, m_centerOffsetX(0.0), m_centerOffsetY(0.0)
	, m_frontRightOffsetX(0.0), m_frontRightOffsetY(0.0)
	, m_cameraId(0), m_packageId(0)
	, m_visionTimeoutMs(30000), m_moveTimeoutMs(7000)
{}

CLoad1VATPerformPickerFOVScanningTask::~CLoad1VATPerformPickerFOVScanningTask() {}

void CLoad1VATPerformPickerFOVScanningTask::OnInitialize(VMF::Context& ctx)
{
	m_cameraId = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_CAMERA_INDEX);
	m_packageId = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_PACKAGE_ID);
	m_moveTimeoutMs = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_MOTION_TIMEOUT_MS);
	m_visionTimeoutMs = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_VISION_TIMEOUT_MS);
	m_fovIntervalX = GetTaskSeqParamAs<double>(ctx, VAT_SEQ_PARAM_PICKER_FOV_INTERVAL_X);
	m_fovIntervalY = GetTaskSeqParamAs<double>(ctx, VAT_SEQ_PARAM_PICKER_FOV_INTERVAL_Y);
	m_currentScanDirection = CENTER;
	m_centerOffsetX = m_centerOffsetY = 0.0;
	m_frontRightOffsetX = m_frontRightOffsetY = 0.0;
	EnterState(MoveSafeZ);
}

VMF::TaskResult CLoad1VATPerformPickerFOVScanningTask::OnPoll(VMF::Context& ctx, VMF::IActuator* actuator)
{
	switch (GetState())
	{
	case MoveSafeZ:          return HandleMoveSafeZ(ctx, actuator);
	case MoveOrigin:         return HandleMoveOrigin(ctx, actuator);
	case MoveScanPosition:   return HandleMoveScanPosition(ctx, actuator);
	case MoveFocusPositionZ: return HandleMoveFocusPositionZ(ctx, actuator);
	case VisionRequest:      return HandleVisionRequest(ctx, actuator);
	case VisionWait:         return HandleVisionWait(ctx, actuator);
	case SaveResult:         return HandleSaveResult(ctx);
	case ReturnHome:         return HandleReturnHome(ctx, actuator);
	default:                 return SetErrorAndReturn(ctx, "Unknown state in CLoad1VATPerformPickerFOVScanningTask");
	}
}

// 스캔 방향에 따른 목표 위치 계산 헬퍼
static void CalcTargetXY(
	CLoad1VATPerformPickerFOVScanningTask::FOVDirection dir,
	double cx, double cy, double dx, double dy,
	double& outX, double& outY)
{
	outX = cx; outY = cy;
	if (dir == CLoad1VATPerformPickerFOVScanningTask::FRONT_RIGHT)
	{
		outX = cx + dx;
		outY = cy - dy;
	}
}

VMF::TaskResult CLoad1VATPerformPickerFOVScanningTask::HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "PickerFOV: actuator is null.");

	if (m_currentScanDirection == CENTER)
	{
		//std::vector<double> pos = actuator->GetEncoder();
		//m_centerPositionX   = pos[0];
		//m_centerPositionY   = pos[1];
		//m_focusPositionZ    = pos[2];
	}

	if (actuator->MoveZ(0.0) != VMF::ActOk) return SetErrorAndReturn(ctx, "PickerFOV: MoveToSafeZ failed.");
	EnterStateWithTimeout(MoveOrigin, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformPickerFOVScanningTask::HandleMoveOrigin(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "PickerFOV: actuator is null.");

	if (actuator->isMoveZ(0.0) != VMF::ActOk)
	{
		if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "PickerFOV: SafeZ timeout.");
		return VMF::TR_KEEP;
	}

	VMF::MotionCommand cmd;
	cmd.Set("X", 0.0);
	cmd.Set("Y", 0.0);
	cmd.pitch = VMF::Narrow;

	if (actuator->Move(cmd) != VMF::ActOk) return SetErrorAndReturn(ctx, "PickerFOV: MoveToOrigin failed.");
	EnterStateWithTimeout(MoveScanPosition, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformPickerFOVScanningTask::HandleMoveScanPosition(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "PickerFOV: actuator is null.");

	VMF::MotionCommand originCmd;
	originCmd.Set("X", 0.0);
	originCmd.Set("Y", 0.0);
	originCmd.pitch = VMF::Narrow;

	if (actuator->isMove(originCmd) != VMF::ActOk)
	{
		if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "PickerFOV: Origin XY timeout.");
		return VMF::TR_KEEP;
	}

	if (m_currentScanDirection != CENTER && m_currentScanDirection != FRONT_RIGHT)
	{
		EnterStateWithTimeout(ReturnHome, m_moveTimeoutMs);
		return VMF::TR_KEEP;
	}

	double targetX, targetY;
	CalcTargetXY(m_currentScanDirection, m_centerPositionX, m_centerPositionY, m_fovIntervalX, m_fovIntervalY, targetX, targetY);

	VMF::MotionCommand cmd;
	cmd.Set("X", targetX);
	cmd.Set("Y", targetY);
	cmd.pitch = VMF::Narrow;

	if (actuator->Move(cmd) != VMF::ActOk) return SetErrorAndReturn(ctx, "PickerFOV: XY Move failed.");
	EnterStateWithTimeout(MoveFocusPositionZ, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformPickerFOVScanningTask::HandleMoveFocusPositionZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "PickerFOV: actuator is null.");

	if (m_currentScanDirection != CENTER && m_currentScanDirection != FRONT_RIGHT)
	{
		EnterStateWithTimeout(ReturnHome, m_moveTimeoutMs);
		return VMF::TR_KEEP;
	}

	double targetX, targetY;
	CalcTargetXY(m_currentScanDirection, m_centerPositionX, m_centerPositionY, m_fovIntervalX, m_fovIntervalY, targetX, targetY);

	VMF::MotionCommand cmd;
	cmd.Set("X", targetX);
	cmd.Set("Y", targetY);
	cmd.pitch = VMF::Narrow;

	if (actuator->isMove(cmd) != VMF::ActOk)
	{
		if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "PickerFOV: Target XY timeout.");
		return VMF::TR_KEEP;
	}

	if (actuator->MoveZ(m_focusPositionZ) != VMF::ActOk) return SetErrorAndReturn(ctx, "PickerFOV: MoveToFocusZ failed.");
	EnterState(VisionRequest);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformPickerFOVScanningTask::HandleVisionRequest(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "PickerFOV: actuator is null.");

	if (actuator->isMoveZ(m_focusPositionZ) != VMF::ActOk)
	{
		if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "PickerFOV: Focus Z timeout.");
		return VMF::TR_KEEP;
	}

	actuator->SetLightState(m_cameraId, true);

	auto visionProcessor = ctx.GetVisionProcessorInterface();
	if (!visionProcessor) return SetErrorAndReturn(ctx, "PickerFOV: No Vision Processor");

	visionProcessor->InitializeRecvThread();
const int visionRequestId = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_VISION_PICKER_FOV_REQUEST_ID);
	if (!ctx.ExecuteVisionCommand(VMF::Measure)) return SetErrorAndReturn(ctx, "PickerFOV: Vision Command Failed");
	EnterStateWithTimeout(VisionWait, m_visionTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformPickerFOVScanningTask::HandleVisionWait(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "PickerFOV: Vision Timeout");

	auto visionProcessor = ctx.GetVisionProcessorInterface();
	if (!visionProcessor) return SetErrorAndReturn(ctx, "PickerFOV: No Vision Processor");
	if (!visionProcessor->IsValid(VMF::Measure)) return VMF::TR_KEEP;

	auto data = visionProcessor->GetLatestData(VMF::Measure);
	double offsetX = 0.0, offsetY = 0.0;
	//auto itX = data.find(VAT_VISION_KEY_X_OFFSET); if (itX != data.end()) offsetX = std::stod(itX->second);
	//auto itY = data.find(VAT_VISION_KEY_Y_OFFSET); if (itY != data.end()) offsetY = std::stod(itY->second);

	if (actuator) actuator->SetLightState(m_cameraId, false);

	if (m_currentScanDirection == CENTER)
	{
		m_centerOffsetX = offsetX;
		m_centerOffsetY = offsetY;
		m_currentScanDirection = FRONT_RIGHT;
		EnterState(MoveSafeZ);
		return VMF::TR_KEEP;
	}

	m_frontRightOffsetX = offsetX;
	m_frontRightOffsetY = offsetY;
	EnterState(SaveResult);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformPickerFOVScanningTask::HandleSaveResult(VMF::Context& ctx)
{
	auto repo = ctx.GetRepository();
	if (!repo) return SetErrorAndReturn(ctx, "PickerFOV: Repository is null.");

	//ctx.SetSeqParamAs<double>("PickerFOVCenterOffsetX", m_centerOffsetX);
	//ctx.SetSeqParamAs<double>("PickerFOVCenterOffsetY", m_centerOffsetY);
	//ctx.SetSeqParamAs<double>("PickerFOVFrontRightOffsetX", m_frontRightOffsetX);
	//ctx.SetSeqParamAs<double>("PickerFOVFrontRightOffsetY", m_frontRightOffsetY);
	//ctx.SetSeqParamAs<double>("PickerFOVFrontRightDeltaX", m_frontRightOffsetX - m_centerOffsetX);
	//ctx.SetSeqParamAs<double>("PickerFOVFrontRightDeltaY", m_frontRightOffsetY - m_centerOffsetY);

	EnterState(ReturnHome);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformPickerFOVScanningTask::HandleReturnHome(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "PickerFOV: actuator is null.");
	if (actuator->MoveZ(0.0) != VMF::ActOk) return SetErrorAndReturn(ctx, "PickerFOV: Z Home Return Fail");
	EnterStateWithTimeout(ReturnHome, m_moveTimeoutMs);
	return VMF::TR_NEXT;
}
