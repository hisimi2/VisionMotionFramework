#include "pch.h"
#include "CLoad1VATPerformFOVScanningTask.h"
#include "DefineVAT.h"
#include "scr\Protocol\VisionMemoryKeys.h"

using namespace VAT_LOAD1::Task;

CLoad1VATPerformFOVScanningTask::CLoad1VATPerformFOVScanningTask()
	: m_centerPositionX(0.0), m_centerPositionY(0.0), m_focusPositionZ(0.0)
	, m_cameraId(0), m_targetPositionX(0.0), m_targetPositionY(0.0)
	, m_fovInterval(2.0), m_visionTimeoutMs(30000), m_moveTimeoutMs(7000)
	, m_currentScanDirection(CENTER)
{}

CLoad1VATPerformFOVScanningTask::~CLoad1VATPerformFOVScanningTask() {}

void CLoad1VATPerformFOVScanningTask::OnInitialize(VMF::Context& ctx)
{
	m_cameraId = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_CAMERA_INDEX);
	m_fovInterval = 2.0;
	m_currentScanDirection = CENTER;
	EnterState(MoveSafeZ);
}

VMF::TaskResult CLoad1VATPerformFOVScanningTask::OnPoll(VMF::Context& ctx, VMF::IActuator* actuator)
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

VMF::TaskResult CLoad1VATPerformFOVScanningTask::HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "Calibration: actuator is null.");

	if (m_currentScanDirection == CENTER)
	{
        /*
		std::vector<double> pos = actuator->GetEncoder();
		m_targetPositionX = m_centerPositionX = pos[0];
		m_targetPositionY = m_centerPositionY = pos[1];
		m_focusPositionZ = pos[2];
		m_currentScanDirection = static_cast<FOVDirection>(static_cast<int>(m_currentScanDirection) + 1);
        */
	}

	if (actuator->MoveZ(0.0) != VMF::ActOk) return SetErrorAndReturn(ctx, "Calibration: MoveToSafeZ failed.");
	EnterStateWithTimeout(MoveOrigin, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformFOVScanningTask::HandleMoveOrigin(VMF::Context& ctx, VMF::IActuator* actuator)
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
	cmd.pitch = VMF::Narrow;

	if (actuator->Move(cmd) != VMF::ActOk) return SetErrorAndReturn(ctx, "Calibration: MoveToOrigin failed.");
	EnterStateWithTimeout(MoveScanPosition, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformFOVScanningTask::HandleMoveScanPosition(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "FOV: actuator is null.");

	VMF::MotionCommand originCmd;
	originCmd.Set("X", 0.0);
	originCmd.Set("Y", 0.0);
	originCmd.pitch = VMF::Narrow;

	if (actuator->isMove(originCmd) != VMF::ActOk)
	{
		if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "FOV: Origin XY timeout.");
		return VMF::TR_KEEP;
	}

	m_targetPositionX = m_centerPositionX;
	m_targetPositionY = m_centerPositionY;

	switch (m_currentScanDirection)
	{
	case REAR:       m_targetPositionY = m_centerPositionY + m_fovInterval; break;
	case FRONT:      m_targetPositionY = m_centerPositionY - m_fovInterval; break;
	case LEFT_SIDE:  m_targetPositionX = m_centerPositionX - m_fovInterval; break;
	case RIGHT_SIDE: m_targetPositionX = m_centerPositionX + m_fovInterval; break;
	default:
		EnterStateWithTimeout(ReturnHome, m_moveTimeoutMs);
		return VMF::TR_KEEP;
	}

	VMF::MotionCommand cmd;
	cmd.Set("X", m_targetPositionX);
	cmd.Set("Y", m_targetPositionY);
	cmd.pitch = VMF::Narrow;

	if (actuator->Move(cmd) != VMF::ActOk) return SetErrorAndReturn(ctx, "XY Move Fail");
	EnterStateWithTimeout(MoveFocusPositionZ, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformFOVScanningTask::HandleMoveFocusPositionZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "FOV: actuator is null.");

	VMF::MotionCommand cmd;
	cmd.Set("X", m_targetPositionX);
	cmd.Set("Y", m_targetPositionY);
	cmd.pitch = VMF::Narrow;

	if (actuator->isMove(cmd) != VMF::ActOk)
	{
		if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "FOV: FocusPos timeout.");
		return VMF::TR_KEEP;
	}

	if (actuator->MoveZ(m_focusPositionZ) != VMF::ActOk) return SetErrorAndReturn(ctx, "FOV: MoveToFocusZ failed.");
	EnterState(VisionRequest);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformFOVScanningTask::HandleVisionRequest(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "FOV: actuator is null.");

	if (actuator->isMoveZ(m_focusPositionZ) != VMF::ActOk)
	{
		if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "Z Move Time Out");
		return VMF::TR_KEEP;
	}

	const int visionRequestId = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_VISION_FOV_REQUEST_ID);
	//ctx.SetSeqParam(VAT_SEQ_PARAM_STATUS, visionRequestId);
	actuator->SetLightState(m_cameraId, true);

	auto visionProcessor = ctx.GetVisionProcessorInterface();
	if (!visionProcessor) return SetErrorAndReturn(ctx, "No Vision Processor");

	visionProcessor->InitializeRecvThread();
	if (!ctx.ExecuteVisionCommand(VMF::Measure)) return SetErrorAndReturn(ctx, "Vision Command Failed");

	EnterStateWithTimeout(VisionWait, m_visionTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformFOVScanningTask::HandleVisionWait(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "Vision Timeout");

	auto visionProcessor = ctx.GetVisionProcessorInterface();
	if (!visionProcessor) return SetErrorAndReturn(ctx, "No Vision Processor");
	if (!visionProcessor->IsValid(VMF::Measure)) return VMF::TR_KEEP;

	auto data = visionProcessor->GetLatestData(VMF::Measure);
	double offsetX = 0.0, offsetY = 0.0;
	//auto itX = data.find(VAT_VISION_KEY_X_OFFSET); if (itX != data.end()) offsetX = std::stod(itX->second);
	//auto itY = data.find(VAT_VISION_KEY_Y_OFFSET); if (itY != data.end()) offsetY = std::stod(itY->second);
	(void)offsetX; (void)offsetY;

	if (actuator) actuator->SetLightState(m_cameraId, false);

	if (static_cast<int>(m_currentScanDirection) < RIGHT_SIDE)
	{
		m_currentScanDirection = static_cast<FOVDirection>(static_cast<int>(m_currentScanDirection) + 1);
		EnterState(MoveSafeZ);
		return VMF::TR_KEEP;
	}

    /*
	if (!ctx.IsVisionPositionEmpty())
	{
		VMF::VisionPosition position;
		ctx.PopVisionPosition(position);
		m_currentScanDirection = CENTER;
		EnterState(VisionRequest);
		return VMF::TR_PREV;
	}
    */

	EnterState(ReturnHome);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformFOVScanningTask::HandleReturnHome(VMF::Context& ctx, VMF::IActuator* actuator)
{
	if (!actuator) return SetErrorAndReturn(ctx, "Z Home Return Fail");
	if (actuator->MoveZ(0.0) != VMF::ActOk) return SetErrorAndReturn(ctx, "Z Home Return Fail");
	return VMF::TR_NEXT;
}
