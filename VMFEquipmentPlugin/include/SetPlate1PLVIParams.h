#pragma once
#include <string>

namespace VMF_PLUGIN
{
    namespace SetPlate1PLVIParams
    {
        // ── 공통 파라미터 (모든 Task에서 사용) ──
        constexpr const char* HAND_ID              = "HandID";
        constexpr const char* PKG_ID               = "PkgID";
        constexpr const char* PLVI_POSITION        = "PLVI_POSITION";
        constexpr const char* TIMEOUT_MEASURE_MS   = "TIMEOUT_MEASURE_MS";
        constexpr const char* TIMEOUT_SCAN_MS      = "TIMEOUT_SCAN_MS";
        constexpr const char* TIMEOUT_RESULT_MS    = "TIMEOUT_RESULT_MS";
        constexpr const char* TIMEOUT_MOVE_MS      = "TIMEOUT_MOVE_MS";
        constexpr const char* SCAN_SPEED_MM_S      = "SCAN_SPEED_MM_S";
        constexpr const char* TRIGGER_INTERVAL_MM  = "TRIGGER_INTERVAL_MM";
        
        // ── 데이터 관련 파라미터 ──
        constexpr const char* DATA_ID              = "DATA_ID";
        constexpr const char* PKG_NAME             = "PKG_NAME";
        
        // ── CTRAY 관련 파라미터 ──
        constexpr const char* CTRAY_X              = "CTRAY_X";
        constexpr const char* CTRAY_Y              = "CTRAY_Y";
        
        // ── Device 정보 (Handler 포켓별) ──
        constexpr const char* DEVICE_INFO_PREFIX   = "DEVICE_INFO_";
        
        // ── ExecuteScan 전용 파라미터 ──
        constexpr const char* SCAN_END_Y           = "SCAN_END_Y";
        
        // ── 기본값 ──
        constexpr int DEFAULT_TIMEOUT_MOVE_MS      = 7000;
        constexpr int DEFAULT_TIMEOUT_RESULT_MS    = 10000;
        constexpr double DEFAULT_SCAN_END_Y        = 200.0;
        constexpr double DEFAULT_TRIGGER_INTERVAL_MM = 1.8;
    }
}