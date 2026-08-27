#pragma once

#include "VisionMotionFramework\IActuator.h"
#include "Actuators\VisionUnitParts.h"

namespace VMF
{
	// ----------------------------------------------------------------
	// ModuleUnitAdapter
	//
	// VisionUnitParts를 IActuator 인터페이스로 래핑.
	// 6면 검사에서 Turn180/Turn360/Gripper/CameraZ 축과
	// 실린더를 제어.
	// ----------------------------------------------------------------
	class ModuleUnitAdapter : public IActuator
	{
		VisionUnitParts* m_parts;
	public:
		explicit ModuleUnitAdapter(VisionUnitParts* parts);
		virtual ~ModuleUnitAdapter();

		// ── IActuator 인터페이스 구현 ──────────────────────────────
		ActError MoveToMeasurementPosition(const VisionPosition& target) override;
		ActError IsAtMeasurementPosition(const VisionPosition& target) override;
		ActError MoveToHomePosition(const VisionPosition& target) override;
		ActError IsAtHomePosition(const VisionPosition& target) override;
		ActError MoveToZ(const VisionPosition& target) override;
		ActError IsAtZ(const VisionPosition& target) override;
		ActError SetLaserState(int laserChannel, bool on, int laserIndex = 0) override;
		ActError GetLaserState(int laserChannel, bool& outOn, int laserIndex = 0) override;
		ActError SetLightState(int camIndex, bool on, int lightIndex = 0) override;
		ActError GetLightState(int camIndex, bool& outOn, int lightIndex = 0) override;
		ActError SetTriggerState(bool enable, double intervalMm, int triggerIndex = 0) override;
		ActError GetTriggerState(bool& outEnabled, double& outIntervalMm, int triggerIndex = 0) override;
		ActError MoveToZSafe(const VisionPosition& target) override;
		ActError IsAtZSafe(const VisionPosition& target) override;
		ActError PrepareForInspection(const VisionPosition& target) override;
		ActError IsAtPrepareForInspection(const VisionPosition& target) override;
		ActError CompleteInspection(const VisionPosition& target) override;
		ActError IsAtCompleteInspection(const VisionPosition& target) override;

		// ── 실린더 제어 (6면 검사 전용) ──────────────────────────
		// (실린더 제어 함수는 IActuator 인터페이스로 통합되어 제거됨)
	};

} // namespace VMF
