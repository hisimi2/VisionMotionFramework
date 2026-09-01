#pragma once

namespace VMF_PLUGIN
{
    namespace PLVI
    {
        namespace Default
        {
            constexpr const char* TIMEOUT_MOVE_MS       = "TIMEOUT_MOVE_MS";
            constexpr const char* TRIGGER_INTERVAL_MM   = "TRIGGER_INTERVAL_MM";
        }

        /// <summary>
        /// Setup Task 파라미터 키
        /// </summary>
        namespace Setup
        {
            constexpr const char* TIMEOUT_MOVE_MS       = "TIMEOUT_MOVE_MS";
            constexpr const char* TRIGGER_INTERVAL_MM   = "TRIGGER_INTERVAL_MM";
        }

        /// <summary>
        /// ExecuteScan Task 파라미터 키
        /// </summary>
        namespace ExecuteScan
        {
            constexpr const char* TIMEOUT_MOVE_MS       = "TIMEOUT_MOVE_MS";
            constexpr const char* TIMEOUT_RESULT_MS     = "TIMEOUT_RESULT_MS";
        }

        /// <summary>
        /// Finish Task 파라미터 키
        /// </summary>
        namespace Finish
        {
            constexpr const char* TIMEOUT_MOVE_MS       = "TIMEOUT_MOVE_MS";
        }
    } // namespace PLVI
} // namespace VMF_PLUGIN
