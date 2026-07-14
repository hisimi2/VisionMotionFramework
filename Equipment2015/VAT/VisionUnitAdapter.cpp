#include "stdafx.h"
#include "VisionUnitAdapter.h"

namespace VMF_6SIDE
{
	VisionUnitAdapter::VisionUnitAdapter(VisionUnitParts* parts)
		: m_parts(parts)
	{}

	VisionUnitAdapter::~VisionUnitAdapter() {}

	VMF::PitchType VisionUnitAdapter::GetPitchType()
	{
		return VMF::PitchType::Variable;
	}

	VMF::ActError VisionUnitAdapter::IsReadyToMove()
	{
		if (!m_parts) return VMF::ActError::ActFail;
		return VMF::ActError::ActOk;
	}

	VMF::ActError VisionUnitAdapter::MoveZ(double targetZ)
	{
		if (!m_parts) return VMF::ActError::ActFail;
		m_parts->CameraZ.Move(targetZ);
		return VMF::ActError::ActOk;
	}

	VMF::ActError VisionUnitAdapter::isMoveZ(double targetZ)
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
	VMF::ActError VisionUnitAdapter::Move(VMF::MotionCommand& cmd)
	{
		if (!m_parts) return VMF::ActError::ActFail;

		double pos = 0.0;
		if (cmd.Get("Turn180"))  m_parts->Turn180.Move(pos);
		if (cmd.Get("Turn360"))  m_parts->Turn360.Move(pos);
		if (cmd.Get("Gripper"))  m_parts->Gripper.Move(pos);
		if (cmd.Get("CameraZ"))  m_parts->CameraZ.Move(pos);

		return VMF::ActError::ActOk;
	}

	VMF::ActError VisionUnitAdapter::isMove(VMF::MotionCommand& cmd)
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

	VMF::ActError VisionUnitAdapter::Stop()
	{
		if (!m_parts) return VMF::ActError::ActFail;
		/*m_parts->Turn180.Stop();
		m_parts->Turn360.Stop();
		m_parts->Gripper.Stop();
		m_parts->CameraZ.Stop();*/
		return VMF::ActError::ActOk;
	}


	int VisionUnitAdapter::SetLightState(int camIndex, bool on)
	{
		if (!m_parts) return -1;
		m_parts->VisionLight.SetStatus(on);
		return 0;
	}

	int VisionUnitAdapter::GetLightState(int camIndex, bool& outOn)
	{
		if (!m_parts) return -1;
		outOn = m_parts->VisionLight.GetStatus();
		return 0;
	}


	VMF::ActError VisionUnitAdapter::SetTriggerState(bool enable, double intervalMm)
	{
		return VMF::ActError::ActFail;
	}

	VMF::ActError VisionUnitAdapter::GetTriggerState(bool& outEnabled, double& outIntervalMm)
	{
		return VMF::ActError::ActFail;
	}

	// ── 실린더 제어 ──────────────────────────────────────────────

	VMF::ActError VisionUnitAdapter::DoCylTurnForBack(bool forward)
	{
		if (!m_parts) return VMF::ActError::ActFail;
		forward ? m_parts->TurnForBack.actA(true)
			: m_parts->TurnForBack.actB(true);
		return VMF::ActError::ActOk;
	}

	VMF::ActError VisionUnitAdapter::ChkCylTurnForBack(bool forward)
	{
		if (!m_parts) return VMF::ActError::ActFail;
		bool done = forward ? m_parts->TurnForBack.actA(true)
			: m_parts->TurnForBack.actB(true);
		return done ? VMF::ActError::ActOk : VMF::ActError::ActBusy;
	}

	VMF::ActError VisionUnitAdapter::DoCylGripUngrip(bool grip)
	{
		if (!m_parts) return VMF::ActError::ActFail;
		grip ? m_parts->GripUngrip.actA(true)
			: m_parts->GripUngrip.actB(true);
		return VMF::ActError::ActOk;
	}

	VMF::ActError VisionUnitAdapter::ChkCylGripUngrip(bool grip)
	{
		if (!m_parts) return VMF::ActError::ActFail;
		bool done = grip ? m_parts->GripUngrip.actA(true)
			: m_parts->GripUngrip.actB(true);
		return done ? VMF::ActError::ActOk : VMF::ActError::ActBusy;
	}

} // namespace VMF_6SIDE
