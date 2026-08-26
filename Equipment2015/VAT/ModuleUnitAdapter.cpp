#include "stdafx.h"
#include "ModuleUnitAdapter.h"

using namespace VMF;

ModuleUnitAdapter::ModuleUnitAdapter(VisionUnitParts* parts)
	: m_parts(parts)
{}

ModuleUnitAdapter::~ModuleUnitAdapter() {}

// ── IActuator 인터페이스 구현 ──────────────────────────────

ActError ModuleUnitAdapter::MoveToMeasurementPosition(const VisionPosition& target)
{
	if (!m_parts) return ActError::ActFail;
	if (target.pos.size() >= 4)
	{
		m_parts->Turn180.Move(target.pos[0]);
		m_parts->Turn360.Move(target.pos[1]);
		m_parts->Gripper.Move(target.pos[2]);
		m_parts->CameraZ.Move(target.pos[3]);
	}
	return ActError::ActOk;
}

ActError ModuleUnitAdapter::IsAtMeasurementPosition(const VisionPosition& target)
{
	if (!m_parts) return ActError::ActFail;
	bool allDone = true;
	if (target.pos.size() >= 4)
	{
		allDone &= m_parts->Turn180.MotionDone();
		allDone &= m_parts->Turn360.MotionDone();
		allDone &= m_parts->Gripper.MotionDone();
		allDone &= m_parts->CameraZ.MotionDone();
	}
	return allDone ? ActError::ActOk : ActError::ActBusy;
}

ActError ModuleUnitAdapter::MoveToHomePosition(const VisionPosition& target)
{
	if (!m_parts) return ActError::ActFail;
	if (target.pos.size() >= 4)
	{
		m_parts->Turn180.Move(target.pos[0]);
		m_parts->Turn360.Move(target.pos[1]);
		m_parts->Gripper.Move(target.pos[2]);
		m_parts->CameraZ.Move(target.pos[3]);
	}
	return ActError::ActOk;
}

ActError ModuleUnitAdapter::IsAtHomePosition(const VisionPosition& target)
{
	if (!m_parts) return ActError::ActFail;
	bool allDone = true;
	if (target.pos.size() >= 4)
	{
		allDone &= m_parts->Turn180.MotionDone();
		allDone &= m_parts->Turn360.MotionDone();
		allDone &= m_parts->Gripper.MotionDone();
		allDone &= m_parts->CameraZ.MotionDone();
	}
	return allDone ? ActError::ActOk : ActError::ActBusy;
}

ActError ModuleUnitAdapter::MoveToZ(const VisionPosition& target)
{
	if (!m_parts) return ActError::ActFail;
	if (target.pos.size() >= 1)
		m_parts->CameraZ.Move(target.pos[0]);
	return ActError::ActOk;
}

ActError ModuleUnitAdapter::IsAtZ(const VisionPosition& target)
{
	if (!m_parts) return ActError::ActFail;
	if (target.pos.size() >= 1)
		return m_parts->CameraZ.MotionDone() ? ActError::ActOk : ActError::ActBusy;
	return ActError::ActOk;
}

ActError ModuleUnitAdapter::SetLaserState(int laserChannel, bool on, int laserIndex)
{
	return ActError::ActFail;
}

ActError ModuleUnitAdapter::GetLaserState(int laserChannel, bool& outOn, int laserIndex)
{
	outOn = false;
	return ActError::ActFail;
}

ActError ModuleUnitAdapter::SetLightState(int camIndex, bool on, int lightIndex)
{
	if (!m_parts) return ActError::ActFail;
	m_parts->VisionLight.SetStatus(on);
	return ActError::ActOk;
}

ActError ModuleUnitAdapter::GetLightState(int camIndex, bool& outOn, int lightIndex)
{
	if (!m_parts) return ActError::ActFail;
	outOn = m_parts->VisionLight.GetStatus();
	return ActError::ActOk;
}

ActError ModuleUnitAdapter::SetTriggerState(bool enable, double intervalMm, int triggerIndex)
{
	return ActError::ActFail;
}

ActError ModuleUnitAdapter::GetTriggerState(bool& outEnabled, double& outIntervalMm, int triggerIndex)
{
	outEnabled = false;
	outIntervalMm = 0.0;
	return ActError::ActFail;
}

ActError ModuleUnitAdapter::MoveToZSafe(const VisionPosition& target)
{
	if (!m_parts) return ActError::ActFail;
	if (target.pos.size() >= 1)
		m_parts->CameraZ.Move(target.pos[0]);
	return ActError::ActOk;
}

ActError ModuleUnitAdapter::IsAtZSafe(const VisionPosition& target)
{
	if (!m_parts) return ActError::ActFail;
	if (target.pos.size() >= 1)
		return m_parts->CameraZ.MotionDone() ? ActError::ActOk : ActError::ActBusy;
	return ActError::ActOk;
}

ActError ModuleUnitAdapter::PrepareForInspection(const VisionPosition& target)
{
	if (!m_parts) return ActError::ActFail;
	m_parts->Gripper.Move(0.0);
	m_parts->GripUngrip.actA(true);
	m_parts->TurnForBack.actA(true);
	return ActError::ActOk;
}

ActError ModuleUnitAdapter::IsAtPrepareForInspection(const VisionPosition& target)
{
	if (!m_parts) return ActError::ActFail;
	if (m_parts->Gripper.GetEncoder() != 0.0)
		return ActError::ActFail;
	if (m_parts->TurnForBack.actA(true))
		return ActError::ActOk;
	if (m_parts->TurnForBack.actB(true))
		return ActError::ActFail;
	return ActError::ActOk;
}

ActError ModuleUnitAdapter::CompleteInspection(const VisionPosition& target)
{
	if (!m_parts) return ActError::ActFail;
	m_parts->GripUngrip.actB(true);
	m_parts->TurnForBack.actB(true);
	return ActError::ActOk;
}

ActError ModuleUnitAdapter::IsAtCompleteInspection(const VisionPosition& target)
{
	if (!m_parts) return ActError::ActFail;
	if (m_parts->GripUngrip.actB(true))
		return ActError::ActOk;
	if (m_parts->GripUngrip.actA(true))
		return ActError::ActFail;
	if (m_parts->TurnForBack.actB(true))
		return ActError::ActOk;
	if (m_parts->TurnForBack.actA(true))
		return ActError::ActFail;
	return ActError::ActFail;
}
