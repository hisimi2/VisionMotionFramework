#pragma once
#include "IActuator.h"
#include "..\Actuators\VisionUnitParts.h"

namespace VMF_6SIDE
{
	// ----------------------------------------------------------------
	// ModuleUnitAdapter
	//
	// VisionUnitParts를 IActuator 인터페이스로 래핑.
	// 6면 검사에서 Turn180/Turn360/Gripper/CameraZ 축과
	// 실린더를 제어.
	// ----------------------------------------------------------------
	class ModuleUnitAdapter : public VMF::IActuator
	{
		VisionUnitParts* m_parts;
	public:
		explicit ModuleUnitAdapter(VisionUnitParts* parts);
		virtual ~ModuleUnitAdapter();

		VMF::PitchType GetPitchType()                        override;
		VMF::ActError  IsReadyToMove()                       override;
		VMF::ActError  MoveZ(double targetZ)                 override;
		VMF::ActError  isMoveZ(double targetZ)               override;
		VMF::ActError  Move(VMF::MotionCommand& cmd)         override;
		VMF::ActError  isMove(VMF::MotionCommand& cmd)       override;
		VMF::ActError  Stop()                                override;


		int  SetLightState(int camIndex, bool on)            override;
		int  GetLightState(int camIndex, bool& outOn)        override;
		VMF::ActError SetTriggerState(bool enable,
			double intervalMm)     override;
		VMF::ActError GetTriggerState(bool& outEnabled,
			double& outIntervalMm) override;

		// ── 실린더 제어 (6면 검사 전용) ──────────────────────────
		VMF::ActError DoCylTurnForBack(bool forward);
		VMF::ActError ChkCylTurnForBack(bool forward);
		VMF::ActError DoCylGripUngrip(bool grip);
		VMF::ActError ChkCylGripUngrip(bool grip);
	};

} // namespace VMF_6SIDE
