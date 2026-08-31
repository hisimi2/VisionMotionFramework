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
        constexpr const char* PLVI_POSITION = "Vision.PLVI_POSITION";

        /// cData[1]: PKG 명칭 (String)
        constexpr const char* PKG_NAME = "Vision.PKG_NAME";

        /// cData[2]: C-Tray 크기 "X,Y" 형태 문자열
        constexpr const char* CTRAY_INFO = "Vision.CTRAY_INFO";

        /// cData[2][0]: C-Tray X 수량 (별도 키, 필요시 사용)
        constexpr const char* CTRAY_X = "Vision.CTRAY_X";

        /// cData[2][1]: C-Tray Y 수량 (별도 키, 필요시 사용)
        constexpr const char* CTRAY_Y = "Vision.CTRAY_Y";

        /// cData[3]: Device 유무 배열 "0,99,99,0,..." 콤마 구분
        constexpr const char* DEVICE_INFO = "Vision.DEVICE_INFO";

        /// cData[3][0] ~: 개별 Device 정보 키 (인덱스별)
        /// 키 형식: "Vision.DEVICE_INFO_0", "Vision.DEVICE_INFO_1", ...
        constexpr const char* DEVICE_INFO_PREFIX = "Vision.DEVICE_INFO_";

        /// nDataID: PLVI 요청 ID
        constexpr const char* DATA_ID = "Vision.DATA_ID";
    }

    // ── PLVI Response Keys ──
    namespace PLVIResult
    {
        /// nDataID: 요청 Req ID + 1000
        constexpr const char* DATA_ID = "Vision.RESULT_DATA_ID";

        /// nStatus: 0=STATUS_ERROR, 1=STATUS_SUCCESS
        constexpr const char* STATUS = "Vision.RESULT_STATUS";

        /// cData[0]: 에러 코드
        /// 값: 0=BUSY, 1=CAM_DISCONNECT, 2=CAM_TIMEOUT, 
        ///      3=CAM_STATUS_FAIL, 4=LIGHT_ERROR, 5=NO_TEACHING_DATA
        constexpr const char* ERROR_CODE = "Vision.RESULT_ERROR_CODE";

        /// cData[1]: 전체 결과
        /// 값: "0"=OK, "1"=NG
        constexpr const char* TOTAL = "Vision.RESULT_TOTAL";

        /// cData[2]: PLVI 위치 (요청 시와 동일한 값, echo)
        constexpr const char* PLVI_POSITION = "Vision.RESULT_PLVI_POSITION";

        /// cData[3]: 개별 Pocket 상태 "0,99,1,2,11,..." 콤마 구분
        constexpr const char* POCKET_RESULT = "Vision.RESULT_POCKET_RESULT";

        /// cData[3][0] ~: 개별 Pocket 결과 키 (인덱스별)
        /// 키 형식: "Vision.RESULT_POCKET_0", "Vision.RESULT_POCKET_1", ...
        constexpr const char* POCKET_PREFIX = "Vision.RESULT_POCKET_";
    }

    // ── VAT Request Keys ──
    namespace VAT
    {
        constexpr const char* RecipeName        = "recipe_name";
        constexpr const char* PcdMode           = "pcd_mode";
        constexpr const char* DeviceSizeX       = "device_size_x";
        constexpr const char* DeviceSizeY       = "device_size_y";
        constexpr const char* CokType           = "cok_type";
        constexpr const char* PickerPitchX      = "picker_pitch_x";
        constexpr const char* PickerPitchY      = "picker_pitch_y";
        constexpr const char* CameraId          = "CameraID";       
        constexpr const char* CameraIndex       = "CameraIndex"; 
        constexpr const char* InspectionType    = "InspectionType";
        constexpr const char* MovePart          = "nMovePart";
        constexpr const char* SaveImage         = "bSaveImage";
        constexpr const char* FovDirection      = "nFovDirection";
    }

    // ── VAT Response Keys ──
    namespace VATResult
    {
        constexpr const char* Result            = "Result";
        constexpr const char* ServerIndex       = "ServerIndex";
        constexpr const char* CamStatus         = "CamStatus";
        constexpr const char* CamType           = "CamType";
        constexpr const char* ZFocusValue       = "ZFocusValue";
        constexpr const char* XOffset           = "XOffset";
        constexpr const char* YOffset           = "YOffset";
        constexpr const char* Angle             = "Angle";
        constexpr const char* AutoVisionSetting = "AutoVisionSetting";
    }

    // ── SixSide Request Keys ──
    namespace SixSide
    {
        constexpr const char* VisionType        = "Side6VisionType";
        // 0=PC1, 1=PC2, 2=PC3, 3=PC4
        constexpr const char* CamPosition       = "CameraIndex";      
        constexpr const char* FacePosition      = "FacePosition";     
        constexpr const char* SelectCount       = "SelectCount";      
        constexpr const char* Skip              = "Side6Skip";        
        constexpr const char* BarcodeId         = "BarcodeID";        
        constexpr const char* LotId             = "LotID";            

        // 호환성을 위한 별칭 (기존 코드 지원)
        constexpr const char* Side6VisionType   = "Side6VisionType";
        constexpr const char* Side6CamPosition  = "Side6CamPosition";
        constexpr const char* Side6FacePosition = "Side6FacePosition";
        constexpr const char* Side6SelectCount  = "Side6SelectCount";
        constexpr const char* Side6Skip         = "Side6Skip";
        constexpr const char* Side6BarcodeId    = "Side6BarcodeId";
        constexpr const char* Side6LotId        = "Side6LotId";
    }

    // ── SixSide Response Keys ──
    namespace SixSideResult
    {
        constexpr const char* GrabCheck = "Side6GrabCheck";
        // "1"=Grab OK, "2"=Grab Fail
        constexpr const char* InspResult = "Side6InspResult";
        // "1"=OK, "2"=NG
        constexpr const char* ResultFace = "Side6ResultFace";
        // 결과 면 번호 echo
    }
}

