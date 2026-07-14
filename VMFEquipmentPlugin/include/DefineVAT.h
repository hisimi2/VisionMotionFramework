#pragma once

// ============================================================================
// [Sample] VAT Sequence Parameter Key 정의
//
// !!! 수정 가이드 !!!
// 1. LocateId (위치 식별자) 값은 장비의 실제 매핑에 맞게 변경
// 2. Sequence Param Key 문자열은 장비의 DB/설정 파일과 일치해야 함
// 3. 필요한 Key가 더 있다면 추가 (예: VAT_SEQ_PARAM_SPEED_HIGH, VAT_SEQ_PARAM_OFFSET_X 등)
// ============================================================================

// --- Sequence Parameter Keys ---

/*
#define VAT_SEQ_PARAM_CAMERA_INDEX               "CameraIndex"
#define VAT_SEQ_PARAM_CAM_INDEX                  "CamIndex"
#define VAT_SEQ_PARAM_PACKAGE_ID                 "PkgID"
#define VAT_SEQ_PARAM_HAND_ID                    "HandID"

#define VAT_SEQ_PARAM_MAX_INSP_COUNT             "maxInspCount"
#define VAT_SEQ_PARAM_MOTION_TIMEOUT_MS          "motion_timeout_ms"
#define VAT_SEQ_PARAM_VISION_TIMEOUT_MS          "vision_timeout_ms"
#define VAT_SEQ_PARAM_TIMEOUT_MS                 "TimeOutMs"

#define VAT_SEQ_PARAM_WIDE_CHECK                 "wide_check"
#define VAT_SEQ_PARAM_INSPECTION_TYPE            "InspectionType"
#define VAT_SEQ_PARAM_MOVE_PART                  "nMovePart"
#define VAT_SEQ_PARAM_STATUS                     "nStatus"

#define VAT_SEQ_PARAM_TARGET_DIFF_X              "TargetDiffX"
#define VAT_SEQ_PARAM_TARGET_DIFF_Y              "TargetDiffY"
*/

#define VAT_SEQ_PARAM_CAMERA_INDEX               "CameraIndex"
#define VAT_SEQ_PARAM_CAM_INDEX                  "CamIndex"
#define VAT_SEQ_PARAM_PACKAGE_ID                 "PkgID"
#define VAT_SEQ_PARAM_HAND_ID                    "HandID"

#define VAT_SEQ_PARAM_MAX_INSP_COUNT             "maxInspCount"
#define VAT_SEQ_PARAM_MOTION_TIMEOUT_MS          "motion_timeout_ms"
#define VAT_SEQ_PARAM_VISION_TIMEOUT_MS          "vision_timeout_ms"
#define VAT_SEQ_PARAM_TIMEOUT_MS                 "TimeOutMs"

#define VAT_SEQ_PARAM_WIDE_CHECK                 "wide_check"
#define VAT_SEQ_PARAM_INSPECTION_TYPE            "InspectionType"
#define VAT_SEQ_PARAM_MOVE_PART                  "nMovePart"
#define VAT_SEQ_PARAM_STATUS                     "nStatus"

#define VAT_SEQ_PARAM_TARGET_DIFF_X              "TargetDiffX"
#define VAT_SEQ_PARAM_TARGET_DIFF_Y              "TargetDiffY"

#define VAT_SEQ_PARAM_PICKER_MAX_ROW            "PickerMaxRow"
#define VAT_SEQ_PARAM_PICKER_MAX_COL            "PickerMaxCol"
#define VAT_SEQ_PARAM_PICKER_GAP_X_NARROW       "PickerGapX_N"
#define VAT_SEQ_PARAM_PICKER_GAP_Y_NARROW       "PickerGapY_N"
#define VAT_SEQ_PARAM_PICKER_GAP_X_WIDE         "PickerGapX_W"
#define VAT_SEQ_PARAM_PICKER_GAP_Y_WIDE         "PickerGapY_W"
#define VAT_SEQ_PARAM_STANDARD_PICKER_COL       "standardPickerCol"
#define VAT_SEQ_PARAM_STANDARD_PICKER_ROW       "standardPickerRow"

#define VAT_SEQ_PARAM_SCAN_AXIS                 "scanAxis"
#define VAT_SEQ_PARAM_ZERO_POS_X                "ZeroPosX"
#define VAT_SEQ_PARAM_ZERO_POS_Y                "ZeroPosY"
#define VAT_SEQ_PARAM_START_POS_X               "StartPosX"
#define VAT_SEQ_PARAM_START_POS_Y               "StartPosY"
#define VAT_SEQ_PARAM_END_POS_X                 "EndPosX"
#define VAT_SEQ_PARAM_END_POS_Y                 "EndPosY"

#define VAT_SEQ_PARAM_PCD_PITCH                 "pcdPitch"

#define VAT_SEQ_PARAM_SAFE_Z                    "safeZ"
#define VAT_SEQ_PARAM_START_X                   "startX"
#define VAT_SEQ_PARAM_START_Y                   "startY"
#define VAT_SEQ_PARAM_FOCUS_Z                   "focusZ"

#define VAT_SEQ_PARAM_VISION_FOV_REQUEST_ID     "VisionFOVRequestID"
#define VAT_SEQ_PARAM_VISION_PICKER_FOV_REQUEST_ID "VisionPickerFOVRequestID"
#define VAT_SEQ_PARAM_PICKER_FOV_INTERVAL_X     "PickerFOVIntervalX"
#define VAT_SEQ_PARAM_PICKER_FOV_INTERVAL_Y     "PickerFOVIntervalY"
#define VAT_SEQ_PARAM_PICKER_FOV_CENTER_OFFSET_X        "PickerFOVCenterOffsetX"
#define VAT_SEQ_PARAM_PICKER_FOV_CENTER_OFFSET_Y        "PickerFOVCenterOffsetY"
#define VAT_SEQ_PARAM_PICKER_FOV_FRONT_RIGHT_OFFSET_X   "PickerFOVFrontRightOffsetX"
#define VAT_SEQ_PARAM_PICKER_FOV_FRONT_RIGHT_OFFSET_Y   "PickerFOVFrontRightOffsetY"
#define VAT_SEQ_PARAM_PICKER_FOV_FRONT_RIGHT_DELTA_X    "PickerFOVFrontRightDeltaX"
#define VAT_SEQ_PARAM_PICKER_FOV_FRONT_RIGHT_DELTA_Y    "PickerFOVFrontRightDeltaY"

#define VAT_SEQ_PARAM_RECIPE_NAME               "recipeName"

// --- LocateId (위치 식별자) ---
// !!! 수정 필요: 장비의 실제 위치 매핑에 맞게 값 변경 !!!
enum LocateId
{
	TargetA     = 1,
	TargetB     = 2,
	UpperTarget = 3,
	MiddleTarget = 4,
	LowerTarget = 5,

	LoadTable1  = 12,
	LoadTable2  = 13,
	LoadTable3  = 14,

	UnloadTable1 = 15,
	UnloadTable2 = 16,
	UnloadTable3 = 17,

	Picker      = 20,
	PickerWide  = 21,
};
