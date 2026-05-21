#pragma once

#include <string>

namespace VisionComm
{
    class ILogger
    {
    public:
        virtual ~ILogger() = default;

        // 간단한 메시지 로깅
        virtual void Log(const std::string& message) = 0;

        // 디버그 레벨 로그
        virtual void LogDebug(const std::string& message) = 0;
    };

} // namespace VisionCommm

