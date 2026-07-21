#pragma once
namespace VMF_PLUGIN
{
	// ================================================================
	// [VAT 공통] Request 파라미터 키
	// ================================================================
	constexpr const char* RECIPE_NAME       = "recipe_name";
	constexpr const char* PCD_MODE          = "pcd_mode";
	constexpr const char* DEVICE_SIZE_X     = "device_size_x";
	constexpr const char* DEVICE_SIZE_Y     = "device_size_y";
	constexpr const char* COK_TYPE          = "cok_type";
	constexpr const char* PICKER_PITCH_X    = "picker_pitch_x";
	constexpr const char* PICKER_PITCH_Y    = "picker_pitch_y";
	constexpr const char* CAMERA_ID         = "CameraID";
	constexpr const char* INSPECTION_TYPE   = "InspectionType";
	constexpr const char* MOVE_PART         = "nMovePart";
	constexpr const char* SAVE_IMAGE        = "bSaveImage";
	constexpr const char* FOV_DIRECTION     = "nFovDirection";

	// ================================================================
	// [VAT 공통] Response 결과 키
	// ================================================================
	constexpr const char* RESULT = "Result";
	constexpr const char* SERVER_INDEX = "ServerIndex";
	constexpr const char* CAM_STATUS = "CamStatus";
	constexpr const char* CAM_TYPE = "CamType";
	constexpr const char* Z_FOCUS_VALUE = "ZFocusValue";
	constexpr const char* X_OFFSET = "XOffset";
	constexpr const char* Y_OFFSET = "YOffset";
	constexpr const char* ANGLE = "Angle";
	constexpr const char* AUTO_VISION_SETTING = "AutoVisionSetting";

	// ================================================================
	// [PLVI] Request 파라미터 키
	// ================================================================
	constexpr const char* PLVI_POSITION = "PlviPosition";
	// 0=SetPlate1, 1=SetPlate2, 11=ShuttleTable1 등
	constexpr const char* PLVI_PKG_NAME = "PlviPkgName";
	constexpr const char* PLVI_CTRAY_X = "PlviCtrayX";
	constexpr const char* PLVI_CTRAY_Y = "PlviCtrayY";
	constexpr const char* PLVI_DEVICE_INFO = "PlviDeviceInfo";
	// "0,99,99,0,..." 형태 콤마 구분 문자열
	// 0=없음, 99=존재

	// ================================================================
	// [PLVI] Response 결과 키
	// ================================================================
	constexpr const char* PLVI_STATUS = "PlviStatus";
	// "0"=ERROR, "1"=SUCCESS
	constexpr const char* PLVI_ERR_CODE = "PlviErrCode";
	// "0"=BUSY, "1"=CAM_DISCONNECT, "2"=CAM_TIMEOUT
	// "3"=CAM_STATUS_FAIL, "4"=LIGHT_ERROR, "5"=NO_TEACHING_DATA
	constexpr const char* PLVI_OVERALL_RESULT = "PlviOverallResult";
	// "0"=OK, "1"=NG
	constexpr const char* PLVI_RESULT_POSITION = "PlviResultPosition";
	// PLVI 위치 echo
	constexpr const char* PLVI_POCKET_RESULT = "PlviPocketResult";
	// "0,99,1,2,11,12,..." 형태 콤마 구분 문자열
	// 0=없음, 99=정상, 1=Leave, 2=Double, 11=Missing, 12=Mismatch

	// ================================================================
	// [6SIDE] Request 파라미터 키
	// ================================================================
	constexpr const char* SIDE6_VISION_TYPE = "Side6VisionType";
	// 0=PC1, 1=PC2, 2=PC3, 3=PC4
	constexpr const char* SIDE6_CAM_POSITION = "Side6CamPosition";
	// 0=CamIndex#1, 1=CamIndex#2
	constexpr const char* SIDE6_FACE_POSITION = "Side6FacePosition";
	// 1=Left, 2=Right, 3=Front, 4=Top, 5=Rear, 6=Bottom
	constexpr const char* SIDE6_SELECT_COUNT = "Side6SelectCount";
	// 검사할 면 총 개수
	constexpr const char* SIDE6_SKIP = "Side6Skip";
	// 0=검사, 1=Skip
	constexpr const char* SIDE6_BARCODE_ID = "Side6BarcodeId";
	constexpr const char* SIDE6_LOT_ID = "Side6LotId";

	// ================================================================
	// [6SIDE] Response 결과 키
	// ================================================================
	constexpr const char* SIDE6_GRAB_CHECK = "Side6GrabCheck";
	// "1"=Grab OK, "2"=Grab Fail
	constexpr const char* SIDE6_INSP_RESULT = "Side6InspResult";
	// "1"=OK, "2"=NG
	constexpr const char* SIDE6_RESULT_FACE = "Side6ResultFace";
	// 결과 면 번호 echo

} // namespace VMF_PLUGIN
