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
    } // namespace PLVI
} // namespace VMF_PLUGIN
