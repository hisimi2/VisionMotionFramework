#include "pch.h"
#include "CSixSideMoveToFacePositionTask.h"
#include "Context.h"
#include "..\..\Equipment2015\VAT\ModuleUnitAdapter.h"

namespace VMF_PLUGIN
{

    

    CSixSideMoveToFacePositionTask::CSixSideMoveToFacePositionTask(VMF::IActuator* actuator)
		: m_needCylOp(false)
		, m_turn180Pos(0.0)
		, m_turn360Pos(0.0)
		, m_cameraZPos(0.0)
		, m_moveTimeoutMs(10000)
		, m_cylTimeoutMs(5000)
	{
        m_actuator = actuator;
    }

	CSixSideMoveToFacePositionTask::~CSixSideMoveToFacePositionTask() {}

	void CSixSideMoveToFacePositionTask::OnInitialize(VMF::Context& ctx)
	{
		// Tag 파라미터에서 이동 정보 읽기
		m_needCylOp = GetTaskSeqParamAs<int>(ctx, "NeedCylOp") != 0;
		m_turn180Pos = GetTaskSeqParamAs<double>(ctx, "Turn180Pos");
		m_turn360Pos = GetTaskSeqParamAs<double>(ctx, "Turn360Pos");
		m_cameraZPos = GetTaskSeqParamAs<double>(ctx, "CameraZPos");

		EnterState(MoveTurn180);
	}

	VMF::TaskResult CSixSideMoveToFacePositionTask::OnPoll(
		VMF::Context& ctx, VMF::IActuator* actuator)
	{
		switch (GetState())
		{
		case MoveTurn180:       return HandleMoveTurn180(ctx, actuator);
		case WaitTurn180:       return HandleWaitTurn180(ctx, actuator);
		case DoCylGrip:         return HandleDoCylGrip(ctx, actuator);
		case WaitCylGrip:       return HandleWaitCylGrip(ctx, actuator);
		case DoGripperSafety:   return HandleDoGripperSafety(ctx, actuator);
		case WaitGripperSafety: return HandleWaitGripperSafety(ctx, actuator);
		case DoCylUngrip:       return HandleDoCylUngrip(ctx, actuator);
		case WaitCylUngrip:     return HandleWaitCylUngrip(ctx, actuator);
		case DoCylBack:         return HandleDoCylBack(ctx, actuator);
		case WaitCylBack:       return HandleWaitCylBack(ctx, actuator);
		case MoveTurn360:       return HandleMoveTurn360(ctx, actuator);
		case WaitTurn360:       return HandleWaitTurn360(ctx, actuator);
		case MoveCameraZ:       return HandleMoveCameraZ(ctx, actuator);
		case WaitCameraZ:       return HandleWaitCameraZ(ctx, actuator);
		case Done:              return VMF::TR_NEXT;
		default:                return SetErrorAndReturn(ctx, "Unknown state");
		}
	}

	// ── Turn180 이동 ─────────────────────────────────────────
	VMF::TaskResult CSixSideMoveToFacePositionTask::HandleMoveTurn180(
		VMF::Context& ctx, VMF::IActuator* actuator)
	{
		if (!actuator) return SetErrorAndReturn(ctx, "No Actuator");

		VMF::MotionCommand cmd;
		cmd.Set("Turn180", m_turn180Pos);
		if (actuator->Move(cmd) != VMF::ActError::ActOk)
			return SetErrorAndReturn(ctx, "Turn180 move failed");

		EnterStateWithTimeout(WaitTurn180, m_moveTimeoutMs);
		return VMF::TR_KEEP;
	}

	VMF::TaskResult CSixSideMoveToFacePositionTask::HandleWaitTurn180(
		VMF::Context& ctx, VMF::IActuator* actuator)
	{
		VMF::MotionCommand cmd;
		cmd.Set("Turn180", m_turn180Pos);
		if (actuator->isMove(cmd) == VMF::ActError::ActBusy)
		{
			if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "Turn180 timeout");
			return VMF::TR_KEEP;
		}

		// 실린더 동작 필요 없으면 바로 CameraZ
		EnterState(m_needCylOp ? DoCylGrip : MoveCameraZ);
		return VMF::TR_KEEP;
	}

// ── 실린더 Grip (Turn360 진입 전 모듈 클램핑) ────────────
	VMF::TaskResult CSixSideMoveToFacePositionTask::HandleDoCylGrip(
		VMF::Context& ctx, VMF::IActuator* actuator)
	{
		auto* moduleActuator = dynamic_cast<VMF_6SIDE::ModuleUnitAdapter*>(actuator);
		if (!moduleActuator) return SetErrorAndReturn(ctx, "Invalid actuator type for cylinder grip");

		if (moduleActuator->DoCylGripUngrip(true) != VMF::ActError::ActOk)
			return SetErrorAndReturn(ctx, "Cyl grip failed");

		EnterStateWithTimeout(WaitCylGrip, m_cylTimeoutMs);
		return VMF::TR_KEEP;
	}

	VMF::TaskResult CSixSideMoveToFacePositionTask::HandleWaitCylGrip(
		VMF::Context& ctx, VMF::IActuator* actuator)
	{
		auto* moduleActuator = dynamic_cast<VMF_6SIDE::ModuleUnitAdapter*>(actuator);
		if (!moduleActuator) return SetErrorAndReturn(ctx, "Invalid actuator type for cylinder grip check");

		if (moduleActuator->ChkCylGripUngrip(true) == VMF::ActError::ActBusy)
		{
			if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "Cyl grip timeout");
			return VMF::TR_KEEP;
		}

		EnterState(DoGripperSafety);
		return VMF::TR_KEEP;
	}

	// ── Gripper Safety 이동 (실린더에 모듈 넘긴 후 후퇴) ─────
	VMF::TaskResult CSixSideMoveToFacePositionTask::HandleDoGripperSafety(
		VMF::Context& ctx, VMF::IActuator* actuator)
	{
		double gripperSafetyPos = GetTaskSeqParamAs<double>(ctx, "GripperSafetyPos");

		VMF::MotionCommand cmd;
		cmd.Set("Gripper", gripperSafetyPos);
		if (actuator->Move(cmd) != VMF::ActError::ActOk)
			return SetErrorAndReturn(ctx, "Gripper safety move failed");

		EnterStateWithTimeout(WaitGripperSafety, m_moveTimeoutMs);
		return VMF::TR_KEEP;
	}

	VMF::TaskResult CSixSideMoveToFacePositionTask::HandleWaitGripperSafety(
		VMF::Context& ctx, VMF::IActuator* actuator)
	{
		double gripperSafetyPos = GetTaskSeqParamAs<double>(ctx, "GripperSafetyPos", 0.0);

		VMF::MotionCommand cmd;
		cmd.Set("Gripper", gripperSafetyPos);
		if (actuator->isMove(cmd) == VMF::ActError::ActBusy)
		{
			if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "Gripper safety timeout");
			return VMF::TR_KEEP;
		}

		EnterState(DoCylUngrip);
		return VMF::TR_KEEP;
	}

// ── 실린더 Ungrip ─────────────────────────────────────────
	VMF::TaskResult CSixSideMoveToFacePositionTask::HandleDoCylUngrip(
		VMF::Context& ctx, VMF::IActuator* actuator)
	{
		auto* moduleActuator = dynamic_cast<VMF_6SIDE::ModuleUnitAdapter*>(actuator);
		if (!moduleActuator) return SetErrorAndReturn(ctx, "Invalid actuator type for cylinder ungrip");

		if (moduleActuator->DoCylGripUngrip(false) != VMF::ActError::ActOk)
			return SetErrorAndReturn(ctx, "Cyl ungrip failed");

		EnterStateWithTimeout(WaitCylUngrip, m_cylTimeoutMs);
		return VMF::TR_KEEP;
	}

	VMF::TaskResult CSixSideMoveToFacePositionTask::HandleWaitCylUngrip(
		VMF::Context& ctx, VMF::IActuator* actuator)
	{
			
		auto* moduleActuator = dynamic_cast<VMF_6SIDE::ModuleUnitAdapter*>(actuator);
		if (!moduleActuator) return SetErrorAndReturn(ctx, "Invalid actuator type for cylinder ungrip check");

		if (moduleActuator->ChkCylGripUngrip(false) == VMF::ActError::ActBusy)
		{
			if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "Cyl ungrip timeout");
			return VMF::TR_KEEP;
		}

		EnterState(DoCylBack);
		return VMF::TR_KEEP;
	}

	// ── 실린더 후진 ───────────────────────────────────────────
	VMF::TaskResult CSixSideMoveToFacePositionTask::HandleDoCylBack(
		VMF::Context& ctx, VMF::IActuator* actuator)
	{
			
		auto* moduleActuator = dynamic_cast<VMF_6SIDE::ModuleUnitAdapter*>(actuator);
		if (!moduleActuator) return SetErrorAndReturn(ctx, "Invalid actuator type for cylinder back");

		if (moduleActuator->DoCylTurnForBack(false) != VMF::ActError::ActOk)
			return SetErrorAndReturn(ctx, "Cyl backward failed");

		EnterStateWithTimeout(WaitCylBack, m_cylTimeoutMs);
		return VMF::TR_KEEP;
	}

	VMF::TaskResult CSixSideMoveToFacePositionTask::HandleWaitCylBack(
		VMF::Context& ctx, VMF::IActuator* actuator)
	{
			
		auto* moduleActuator = dynamic_cast<VMF_6SIDE::ModuleUnitAdapter*>(actuator);
		if (!moduleActuator) return SetErrorAndReturn(ctx, "Invalid actuator type for cylinder back check");

		if (moduleActuator->ChkCylTurnForBack(false) == VMF::ActError::ActBusy)
		{
			if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "Cyl backward timeout");
			return VMF::TR_KEEP;
		}

		EnterState(MoveTurn360);
		return VMF::TR_KEEP;
	}

	// ── Turn360 이동 ─────────────────────────────────────────
	VMF::TaskResult CSixSideMoveToFacePositionTask::HandleMoveTurn360(
		VMF::Context& ctx, VMF::IActuator* actuator)
	{
		VMF::MotionCommand cmd;
		cmd.Set("Turn360", m_turn360Pos);
		if (actuator->Move(cmd) != VMF::ActError::ActOk)
			return SetErrorAndReturn(ctx, "Turn360 move failed");

		EnterStateWithTimeout(WaitTurn360, m_moveTimeoutMs);
		return VMF::TR_KEEP;
	}

	VMF::TaskResult CSixSideMoveToFacePositionTask::HandleWaitTurn360(
		VMF::Context& ctx, VMF::IActuator* actuator)
	{
		VMF::MotionCommand cmd;
		cmd.Set("Turn360", m_turn360Pos);
		if (actuator->isMove(cmd) == VMF::ActError::ActBusy)
		{
			if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "Turn360 timeout");
			return VMF::TR_KEEP;
		}

		EnterState(MoveCameraZ);
		return VMF::TR_KEEP;
	}

	// ── CameraZ 이동 ─────────────────────────────────────────
	VMF::TaskResult CSixSideMoveToFacePositionTask::HandleMoveCameraZ(
		VMF::Context& ctx, VMF::IActuator* actuator)
	{
		if (actuator->MoveZ(m_cameraZPos) != VMF::ActError::ActOk)
			return SetErrorAndReturn(ctx, "CameraZ move failed");

		EnterStateWithTimeout(WaitCameraZ, m_moveTimeoutMs);
		return VMF::TR_KEEP;
	}

	VMF::TaskResult CSixSideMoveToFacePositionTask::HandleWaitCameraZ(
		VMF::Context& ctx, VMF::IActuator* actuator)
	{
		if (actuator->isMoveZ(m_cameraZPos) == VMF::ActError::ActBusy)
		{
			if (IsDeadlineExpired()) return SetErrorAndReturn(ctx, "CameraZ timeout");
			return VMF::TR_KEEP;
		}

		EnterState(Done);
		return VMF::TR_KEEP;
	}

} // namespace VMF_PLUGIN_6SIDE
