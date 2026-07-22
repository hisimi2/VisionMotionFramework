#include "stdafx.h"
#include "ModuleUnitAdapter.h"

namespace VMF_6SIDE
{
	ModuleUnitAdapter::ModuleUnitAdapter(VisionUnitParts* parts)
		: m_parts(parts)
	{}

	ModuleUnitAdapter::~ModuleUnitAdapter() {}

	VMF::PitchType ModuleUnitAdapter::GetPitchType()
	{
		return VMF::PitchType::Variable;
	}

	VMF::ActError ModuleUnitAdapter::IsReadyToMove()
	{
		if (!m_parts) return VMF::ActError::ActFail;
		return VMF::ActError::ActOk;
	}

	VMF::ActError ModuleUnitAdapter::MoveZ(double targetZ)
	{
		if (!m_parts) return VMF::ActError::ActFail;
		m_parts->CameraZ.Move(targetZ);
		return VMF::ActError::ActOk;
	}

	VMF::ActError ModuleUnitAdapter::isMoveZ(double targetZ)
	{
		if (!m_parts) return VMF::ActError::ActFail;
		return m_parts->CameraZ.MotionDone()
			? VMF::ActError::ActOk
			: VMF::ActError::ActBusy;
	}

	// MotionCommand 축 이름 규칙:
	//   "Turn180"  ? Turn180 축
	//   "Turn360"  ? Turn360 축
	//   "Gripper"  ? Gripper 축
	//   "CameraZ"  ? CameraZ 축
	VMF::ActError ModuleUnitAdapter::Move(VMF::MotionCommand& cmd)
	{
		if (!m_parts) return VMF::ActError::ActFail;

		double pos = 0.0;
		if (cmd.Get("Turn180"))  m_parts->Turn180.Move(pos);
		if (cmd.Get("Turn360"))  m_parts->Turn360.Move(pos);
		if (cmd.Get("Gripper"))  m_parts->Gripper.Move(pos);
		if (cmd.Get("CameraZ"))  m_parts->CameraZ.Move(pos);

		return VMF::ActError::ActOk;
	}

	VMF::ActError ModuleUnitAdapter::isMove(VMF::MotionCommand& cmd)
	{
		if (!m_parts) return VMF::ActError::ActFail;

		bool allDone = true;

		if (cmd.Get("Turn180"))
			allDone &= m_parts->Turn180.MotionDone();
		if (cmd.Get("Turn360"))
			allDone &= m_parts->Turn360.MotionDone();
		if (cmd.Get("Gripper"))
			allDone &= m_parts->Gripper.MotionDone();
		if (cmd.Get("CameraZ"))
			allDone &= m_parts->CameraZ.MotionDone();

		return allDone ? VMF::ActError::ActOk : VMF::ActError::ActBusy;
	}

	VMF::ActError ModuleUnitAdapter::Stop()
	{
		if (!m_parts) return VMF::ActError::ActFail;
		/*m_parts->Turn180.Stop();
		m_parts->Turn360.Stop();
		m_parts->Gripper.Stop();
		m_parts->CameraZ.Stop();*/
		return VMF::ActError::ActOk;
	}


	int ModuleUnitAdapter::SetLightState(int camIndex, bool on)
	{
		if (!m_parts) return -1;
		m_parts->VisionLight.SetStatus(on);
		return 0;
	}

	int ModuleUnitAdapter::GetLightState(int camIndex, bool& outOn)
	{
		if (!m_parts) return -1;
		outOn = m_parts->VisionLight.GetStatus();
		return 0;
	}


	VMF::ActError ModuleUnitAdapter::SetTriggerState(bool enable, double intervalMm)
	{
		return VMF::ActError::ActFail;
	}

	VMF::ActError ModuleUnitAdapter::GetTriggerState(bool& outEnabled, double& outIntervalMm)
	{
		return VMF::ActError::ActFail;
	}

	// ── 실린더 제어 ──────────────────────────────────────────────

	VMF::ActError ModuleUnitAdapter::DoCylTurnForBack(bool forward)
	{
		if (!m_parts) return VMF::ActError::ActFail;
		forward ? m_parts->TurnForBack.actA(true)
			: m_parts->TurnForBack.actB(true);
		return VMF::ActError::ActOk;
	}

	VMF::ActError ModuleUnitAdapter::ChkCylTurnForBack(bool forward)
	{
		if (!m_parts) return VMF::ActError::ActFail;
		bool done = forward ? m_parts->TurnForBack.actA(true)
			: m_parts->TurnForBack.actB(true);
		return done ? VMF::ActError::ActOk : VMF::ActError::ActBusy;
	}

	VMF::ActError ModuleUnitAdapter::DoCylGripUngrip(bool grip)
	{
		if (!m_parts) return VMF::ActError::ActFail;
		grip ? m_parts->GripUngrip.actA(true)
			: m_parts->GripUngrip.actB(true);
		return VMF::ActError::ActOk;
	}

	VMF::ActError ModuleUnitAdapter::ChkCylGripUngrip(bool grip)
	{
		if (!m_parts) return VMF::ActError::ActFail;
		bool done = grip ? m_parts->GripUngrip.actA(true)
			: m_parts->GripUngrip.actB(true);
		return done ? VMF::ActError::ActOk : VMF::ActError::ActBusy;
	}

} // namespace VMF_6SIDE
