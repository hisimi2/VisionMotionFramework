#pragma once

#include <vector>
#include <string>

namespace VisionComm { enum VisionStatus : int; } // 전방 선언 시 기본 underlying type 명시 (선택사항이나 C++11에 적합성 높임)

namespace DVH_VAT
{
    class IResultSink
    {
    public:
        virtual ~IResultSink() = default;

        // called when a new result is available
        virtual void NotifyVisionResult(int requestId, const std::vector<std::string>& results) = 0;
    };
}

