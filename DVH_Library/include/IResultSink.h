#pragma once

#include <vector>
#include <string>

namespace VisionCom { enum VisionStatus : int; } // 전방 선언 시 기본 underlying type 명시 (선택사항이나 C++11에 적합성 높임)

namespace DVH_VAT
{
    class IResultSink
    {
    public:
        // C++11/14: 다형성 인터페이스의 가상 소멸자에 명시적으로 = default 적용
        virtual ~IResultSink() = default;

        // called when a new result is available
        virtual void NotifyVisionResult(int requestId, const std::vector<std::string>& results) = 0;
    };
}
