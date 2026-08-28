#pragma once

namespace VMF_PLUGIN
{
    // ── PLVI Request Keys ──
    namespace PLVI
    {
        constexpr const char* Position = "PLVI_POSITION";      // Builder/Task에서 실제 사용 키
        constexpr const char* PkgName = "PLVI_PKG_NAME";
        constexpr const char* CtrayX = "CTRAY_X";
        constexpr const char* CtrayY = "CTRAY_Y";
        constexpr const char* DeviceInfo = "PLVI_DEVICE_INFO";
        // "0,99,99,0,..." 형태 콤마 구분 문자열 (0=없음, 99=존재)
    }

    // ── PLVI Response Keys ──
    namespace PLVIResult
    {
        constexpr const char* Status = "PlviStatus";
        // "0"=ERROR, "1"=SUCCESS
        constexpr const char* ErrCode = "PlviErrCode";
        // "0"=BUSY, "1"=CAM_DISCONNECT, "2"=CAM_TIMEOUT
        // "3"=CAM_STATUS_FAIL, "4"=LIGHT_ERROR, "5"=NO_TEACHING_DATA
        constexpr const char* OverallResult = "PlviOverallResult";
        // "0"=OK, "1"=NG
        constexpr const char* ResultPosition = "PlviResultPosition";
        // PLVI 위치 echo
        constexpr const char* PocketResult = "PlviPocketResult";
        // "0,99,1,2,11,12,..." 형태 콤마 구분 문자열
        // 0=없음, 99=정상, 1=Leave, 2=Double, 11=Missing, 12=Mismatch
    }

    // ── VAT Request Keys ──
    namespace VAT
    {
        constexpr const char* RecipeName = "recipe_name";
        constexpr const char* PcdMode = "pcd_mode";
        constexpr const char* DeviceSizeX = "device_size_x";
        constexpr const char* DeviceSizeY = "device_size_y";
        constexpr const char* CokType = "cok_type";
        constexpr const char* PickerPitchX = "picker_pitch_x";
        constexpr const char* PickerPitchY = "picker_pitch_y";
        constexpr const char* CameraId = "CameraID";       // Device ID (3028, 3128, 3130) - used in VisionVatProcessor
        constexpr const char* CameraIndex = "CameraIndex";    // Camera index (0, 1, 6, 7) - used for camera selection
        constexpr const char* InspectionType = "InspectionType";
        constexpr const char* MovePart = "nMovePart";
        constexpr const char* SaveImage = "bSaveImage";
        constexpr const char* FovDirection = "nFovDirection";
    }

    // ── VAT Response Keys ──
    namespace VATResult
    {
        constexpr const char* Result = "Result";
        constexpr const char* ServerIndex = "ServerIndex";
        constexpr const char* CamStatus = "CamStatus";
        constexpr const char* CamType = "CamType";
        constexpr const char* ZFocusValue = "ZFocusValue";
        constexpr const char* XOffset = "XOffset";
        constexpr const char* YOffset = "YOffset";
        constexpr const char* Angle = "Angle";
        constexpr const char* AutoVisionSetting = "AutoVisionSetting";
    }


    // ── SixSide Request Keys ──
    namespace SixSide
    {
        constexpr const char* VisionType = "Side6VisionType";
        // 0=PC1, 1=PC2, 2=PC3, 3=PC4
        constexpr const char* CamPosition = "CameraIndex";      // Builder/Task에서 실제 사용 키
        constexpr const char* FacePosition = "FacePosition";     // Builder/Task에서 실제 사용 키
        constexpr const char* SelectCount = "SelectCount";      // Builder/Task에서 실제 사용 키
        constexpr const char* Skip = "Side6Skip";        // Builder/Task에서 실제 사용 키 (일부 Task에서 사용)
        constexpr const char* BarcodeId = "BarcodeID";        // Builder/Task에서 실제 사용 키
        constexpr const char* LotId = "LotID";            // Builder/Task에서 실제 사용 키

        // 호환성을 위한 별칭 (기존 코드 지원)
        constexpr const char* Side6VisionType = "Side6VisionType";
        constexpr const char* Side6CamPosition = "Side6CamPosition";
        constexpr const char* Side6FacePosition = "Side6FacePosition";
        constexpr const char* Side6SelectCount = "Side6SelectCount";
        constexpr const char* Side6Skip = "Side6Skip";
        constexpr const char* Side6BarcodeId = "Side6BarcodeId";
        constexpr const char* Side6LotId = "Side6LotId";
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

