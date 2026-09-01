#pragma once

namespace VMF_PLUGIN
{
    namespace PLVI
    {
        // ──────────────────────────────────────────────────────────────
        // REQ_MEASURE 요청 (S107/F5, H→V) - cData 구성
        // ──────────────────────────────────────────────────────────────

        /// cData[0]: PLVI 위치
        /// 값: 0=Set-Plate1, 1=Set-Plate2, 11=Shuttle Table1, 12=Shuttle Table2,
        /// 21=Retest Buffer1, 22=Retest Buffer2
        constexpr const char* PLVI_POSITION = "PLVI_POSITION";

        /// cData[1]: PKG 명칭 (String)
        constexpr const char* PKG_NAME = "PKG_NAME";

        /// cData[2]: C-Tray 크기 "X,Y" 형태 문자열
        /// (CTRAY_INFO는 CTRAY_X, CTRAY_Y 개별 키로 대체되어 미사용)
        // constexpr const char* CTRAY_INFO = "CTRAY_INFO";  // 미사용 - 제거 권장

        /// cData[2][0]: C-Tray X 수량 (별도 키, 필요시 사용)
        constexpr const char* CTRAY_X = "CTRAY_X";

        /// cData[2][1]: C-Tray Y 수량 (별도 키, 필요시 사용)
        constexpr const char* CTRAY_Y = "CTRAY_Y";

        /// cData[3]: Device 유무 배열 "0,99,99,0,..." 콤마 구분
        constexpr const char* DEVICE_INFO = "DEVICE_INFO";

        /// cData[3][0] ~: 개별 Device 정보 키 (인덱스별)
        /// 키 형식: "DEVICE_INFO_0", "DEVICE_INFO_1", ...
        constexpr const char* DEVICE_INFO_PREFIX = "DEVICE_INFO_";

        /// nDataID: PLVI 요청 ID
        constexpr const char* DATA_ID = "DATA_ID";
    }

    // ── PLVI Response Keys ──
    namespace PLVIResult
    {
        /// nDataID: 요청 Req ID + 1000
        constexpr const char* DATA_ID = "RESULT_DATA_ID";

        /// nStatus: 0=STATUS_ERROR, 1=STATUS_SUCCESS
        constexpr const char* STATUS = "RESULT_STATUS";

        /// cData[0]: 에러 코드
        /// 값: 0=BUSY, 1=CAM_DISCONNECT, 2=CAM_TIMEOUT, 
        ///      3=CAM_STATUS_FAIL, 4=LIGHT_ERROR, 5=NO_TEACHING_DATA
        constexpr const char* ERROR_CODE = "RESULT_ERROR_CODE";

        /// cData[1]: 전체 결과
        /// 값: "0"=OK, "1"=NG
        constexpr const char* TOTAL = "RESULT_TOTAL";

        /// cData[2]: PLVI 위치 (요청 시와 동일한 값, echo)
        constexpr const char* PLVI_POSITION = "RESULT_PLVI_POSITION";

        /// cData[3]: 개별 Pocket 상태 "0,99,1,2,11,..." 콤마 구분
        constexpr const char* POCKET_RESULT = "RESULT_POCKET_RESULT";

        /// cData[3][0] ~: 개별 Pocket 결과 키 (인덱스별)
        /// 키 형식: "RESULT_POCKET_0", "RESULT_POCKET_1", ...
        constexpr const char* POCKET_PREFIX = "RESULT_POCKET_";
    }

    // ── VAT Request Keys ──
    namespace VAT
    {
        constexpr const char* RECIPE_NAME = "RECIPE_NAME";
        constexpr const char* PCD_MODE = "PCD_MODE";
        constexpr const char* DEVICE_SIZE_X = "DEVICE_SIZE_X";
        constexpr const char* DEVICE_SIZE_Y = "DEVICE_SIZE_Y";
        constexpr const char* COK_TYPE = "COK_TYPE";
        constexpr const char* PICKER_PITCH_X = "PICKER_PITCH_X";
        constexpr const char* PICKER_PITCH_Y = "PICKER_PITCH_Y";
        constexpr const char* CAMERA_ID = "CAMERA_ID";
        constexpr const char* CAMERA_INDEX = "CAMERA_INDEX";
        constexpr const char* INSPECTION_TYPE = "INSPECTION_TYPE";
        constexpr const char* MOVE_PART = "MOVE_PART";
        constexpr const char* SAVE_IMAGE = "SAVE_IMAGE";
        constexpr const char* FOV_DIRECTION = "FOV_DIRECTION";
    }

    // ── VAT Response Keys ──
    namespace VATResult
    {
        constexpr const char* RESULT = "RESULT";
        constexpr const char* SERVER_INDEX = "SERVER_INDEX";
        constexpr const char* CAM_STATUS = "CAM_STATUS";
        constexpr const char* CAM_TYPE = "CAM_TYPE";
        constexpr const char* Z_FOCUS_VALUE = "Z_FOCUS_VALUE";
        constexpr const char* X_OFFSET = "X_OFFSET";
        constexpr const char* Y_OFFSET = "Y_OFFSET";
        constexpr const char* ANGLE = "ANGLE";
        constexpr const char* AUTO_VISION_SETTING = "AUTO_VISION_SETTING";
    }

    // ── SixSide Request Keys ──
    namespace SixSide
    {
        constexpr const char* VISION_TYPE = "SIDE6_VISION_TYPE";
        // 0=PC1, 1=PC2, 2=PC3, 3=PC4
        constexpr const char* CAM_POSITION = "CAMERA_INDEX";
        constexpr const char* FACE_POSITION = "FACE_POSITION";
        constexpr const char* SELECT_COUNT = "SELECT_COUNT";
        constexpr const char* SKIP = "SIDE6_SKIP";
        constexpr const char* BARCODE_ID = "BARCODE_ID";
        constexpr const char* LOT_ID = "LOT_ID";

        // 호환성을 위한 별칭 (기존 코드 지원)
        constexpr const char* SIDE6_VISION_TYPE = "SIDE6_VISION_TYPE";
        constexpr const char* SIDE6_CAM_POSITION = "SIDE6_CAM_POSITION";
        constexpr const char* SIDE6_FACE_POSITION = "SIDE6_FACE_POSITION";
        constexpr const char* SIDE6_SELECT_COUNT = "SIDE6_SELECT_COUNT";
        constexpr const char* SIDE6_SKIP = "SIDE6_SKIP";
        constexpr const char* SIDE6_BARCODE_ID = "SIDE6_BARCODE_ID";
        constexpr const char* SIDE6_LOT_ID = "SIDE6_LOT_ID";
    }

    // ── SixSide Response Keys ──
    namespace SixSideResult
    {
        constexpr const char* GRAB_CHECK = "SIDE6_GRAB_CHECK";
        // "1"=Grab OK, "2"=Grab Fail
        constexpr const char* INSP_RESULT = "SIDE6_INSP_RESULT";
        // "1"=OK, "2"=NG
        constexpr const char* RESULT_FACE = "SIDE6_RESULT_FACE";
        // 결과 면 번호 echo
    }
}

