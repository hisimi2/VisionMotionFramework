#pragma once
#include "IActuator.h"
#include "..\Actuators\VisionUnitParts.h"

namespace VMF
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

		// ── IActuator 인터페이스 구현 ──────────────────────────────
		VMF::ActError MoveToMeasurementPosition(const VMF::VisionPosition& target) override;
		VMF::ActError IsAtMeasurementPosition(const VMF::VisionPosition& target) override;
		VMF::ActError MoveToHomePosition(const VMF::VisionPosition& target) override;
		VMF::ActError IsAtHomePosition(const VMF::VisionPosition& target) override;
		VMF::ActError MoveToZ(const VMF::VisionPosition& target) override;
		VMF::ActError IsAtZ(const VMF::VisionPosition& target) override;
		VMF::ActError SetLaserState(int laserChannel, bool on, int laserIndex = 0) override;
		VMF::ActError GetLaserState(int laserChannel, bool& outOn, int laserIndex = 0) override;
		VMF::ActError SetLightState(int camIndex, bool on, int lightIndex = 0) override;
		VMF::ActError GetLightState(int camIndex, bool& outOn, int lightIndex = 0) override;
		VMF::ActError SetTriggerState(bool enable, double intervalMm, int triggerIndex = 0) override;
		VMF::ActError GetTriggerState(bool& outEnabled, double& outIntervalMm, int triggerIndex = 0) override;
		VMF::ActError MoveToZSafe(const VMF::VisionPosition& target) override;
		VMF::ActError IsAtZSafe(const VMF::VisionPosition& target) override;
		VMF::ActError PrepareForInspection(const VMF::VisionPosition& target) override;
		VMF::ActError IsAtPrepareForInspection(const VMF::VisionPosition& target) override;
		VMF::ActError CompleteInspection(const VMF::VisionPosition& target) override;
		VMF::ActError IsAtCompleteInspection(const VMF::VisionPosition& target) override;

		// ── 실린더 제어 (6면 검사 전용) ──────────────────────────
		// (실린더 제어 함수는 IActuator 인터페이스로 통합되어 제거됨)
	};

} // namespace VMF_6SIDE
