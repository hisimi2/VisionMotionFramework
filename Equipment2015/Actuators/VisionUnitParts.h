#pragma once
#include "Actuators\CAxisController.h"
#include "Actuators\CylinderTypes.h"
#include "Actuators\CLamp.h"

// ----------------------------------------------------------------
// VisionUnitParts
//
// 6면 검사 장비 구성 요소.
// Turn180/Turn360 축으로 모듈을 회전시키며
// CameraZ 축으로 카메라 위치를 조정하여 6면 촬영.
// ----------------------------------------------------------------
class VisionUnitParts
{
public:
	VisionUnitParts::VisionUnitParts()
		: Turn180("VisionUnit_Turn180")
		, Turn360("VisionUnit_Turn360")
		, Gripper("VisionUnit_Gripper")
		, CameraZ("VisionUnit_CameraZ")
		, TurnForBack("VisionUnit_TurnForBack")
		, GripUngrip("VisionUnit_GripUngrip")
		, VisionLight("VisionUnit_VisionLight")
	{}

	VisionUnitParts::~VisionUnitParts() {}

	// ── 모터 축 ──────────────────────────────────────────────────
	OneAxis     Turn180;        // 좌/우 전환 (Left/Right/Safety)
	OneAxis     Turn360;        // 전/후/상/하 전환 (Front/Top/Rear/Bottom)
	OneAxis     Gripper;        // 모듈 파지 (Grip/Ungrip/Safety)
	OneAxis     CameraZ;        // 카메라 Z축 이동 (면별 포지션)

								// ── 실린더 ───────────────────────────────────────────────────
	CForBackCylinder        TurnForBack;    // Turn360 회전 시 전/후 고정
	CClampReleaseCylinder   GripUngrip;     // 모듈 클램핑/언클램핑

											// ── 조명 ─────────────────────────────────────────────────────
	CLamp       VisionLight;
};