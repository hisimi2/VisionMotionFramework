#pragma once

namespace VMF_PLUGIN
{
    // ── PLVI Request Keys ──
    namespace PLVI
    {
        constexpr const char* Position     = "PLVI_POSITION";      // Builder/Task에서 실제 사용 키
        constexpr const char* PkgName      = "PLVI_PKG_NAME";
        constexpr const char* CtrayX       = "CTRAY_X";
        constexpr const char* CtrayY       = "CTRAY_Y";
        constexpr const char* DeviceInfo   = "PLVI_DEVICE_INFO";
        // "0,99,99,0,..." 형태 콤마 구분 문자열 (0=없음, 99=존재)
    }

    // ── PLVI Response Keys ──
    namespace PLVIResult
    {
        constexpr const char* Status         = "PlviStatus";
        // "0"=ERROR, "1"=SUCCESS
        constexpr const char* ErrCode        = "PlviErrCode";
        // "0"=BUSY, "1"=CAM_DISCONNECT, "2"=CAM_TIMEOUT
        // "3"=CAM_STATUS_FAIL, "4"=LIGHT_ERROR, "5"=NO_TEACHING_DATA
        constexpr const char* OverallResult  = "PlviOverallResult";
        // "0"=OK, "1"=NG
        constexpr const char* ResultPosition = "PlviResultPosition";
        // PLVI 위치 echo
        constexpr const char* PocketResult   = "PlviPocketResult";
        // "0,99,1,2,11,12,..." 형태 콤마 구분 문자열
        // 0=없음, 99=정상, 1=Leave, 2=Double, 11=Missing, 12=Mismatch
    }
}