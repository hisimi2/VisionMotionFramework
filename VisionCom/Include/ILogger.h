#pragma once

#include <string>

namespace VisionCom
{
    class ILogger 
    {
    public:
        // C++11/14: 비어있는 다형성 가상 소멸자는 = default 로 구현을 명시
        virtual ~ILogger() = default;

        // 간단한 메시지 로깅
        virtual void Log(const std::string& message) = 0;

        // 디버그 레벨 로그
        virtual void LogDebug(const std::string& message) = 0;
    };

} // namespace VisionCom
