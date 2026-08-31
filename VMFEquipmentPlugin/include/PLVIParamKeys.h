#pragma once

#include <string>

namespace VMF_PLUGIN
{
    namespace PLVI
    {
        namespace Default
        {
            constexpr const char* TIMEOUT_MOVE_MS = "Setup.TIMEOUT_MOVE_MS";
            constexpr const char* TRIGGER_INTERVAL_MM = "Setup.TRIGGER_INTERVAL_MM";
            constexpr const char* START_POS_X = "Setup.START_POS_X";
            constexpr const char* START_POS_Y = "Setup.START_POS_Y";
            constexpr const char* START_POS_Z = "Setup.START_POS_Z";
        }

        /// <summary>
        /// Setup Task 파라미터 키
        /// </summary>
        namespace Setup
        {
            constexpr const char* TIMEOUT_MOVE_MS = "Setup.TIMEOUT_MOVE_MS";
            constexpr const char* TRIGGER_INTERVAL_MM = "Setup.TRIGGER_INTERVAL_MM";
        }

        /// <summary>
        /// ExecuteScan Task 파라미터 키
        /// </summary>
        namespace ExecuteScan
        {
            constexpr const char* TIMEOUT_MOVE_MS = "ExecuteScan.TIMEOUT_MOVE_MS";
            constexpr const char* TIMEOUT_RESULT_MS = "ExecuteScan.TIMEOUT_RESULT_MS";
        }

        /// <summary>
        /// Finish Task 파라미터 키
        /// </summary>
        namespace Finish
        {
            constexpr const char* TIMEOUT_MOVE_MS = "Finish.TIMEOUT_MOVE_MS";
        }

        /// <summary>
        /// Vision 통신 파라미터 키 (비전 프로세서와 통신 시 사용)
        /// </summary>
        namespace Vision
        {
            constexpr const char* HAND_ID = "Vision.HAND_ID";
            constexpr const char* PKG_ID = "Vision.PKG_ID";
            constexpr const char* PLVI_POSITION = "Vision.PLVI_POSITION";
            constexpr const char* TIMEOUT_MEASURE_MS = "Vision.TIMEOUT_MEASURE_MS";
            constexpr const char* TIMEOUT_SCAN_MS = "Vision.TIMEOUT_SCAN_MS";
            constexpr const char* TIMEOUT_RESULT_MS = "Vision.TIMEOUT_RESULT_MS";
            constexpr const char* TIMEOUT_MOVE_MS = "Vision.TIMEOUT_MOVE_MS";
            constexpr const char* SCAN_SPEED_MM_S = "Vision.SCAN_SPEED_MM_S";
            constexpr const char* TRIGGER_INTERVAL_MM = "Vision.TRIGGER_INTERVAL_MM";
            constexpr const char* DATA_ID = "Vision.DATA_ID";
            constexpr const char* PKG_NAME = "Vision.PKG_NAME";
            constexpr const char* CTRAY_X = "Vision.CTRAY_X";
            constexpr const char* CTRAY_Y = "Vision.CTRAY_Y";
            constexpr const char* DEVICE_INFO_PREFIX = "Vision.DEVICE_INFO_PREFIX";
        }
    } // namespace ParamKeys
} // namespace VMF_PLUGIN
