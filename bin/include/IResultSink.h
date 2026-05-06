#pragma once

#include <vector>
#include <string>

namespace VisionCom { enum VisionStatus; }

namespace DVH_VAT
{
    class IResultSink
    {
    public:
        virtual ~IResultSink() {}

        // called when a new result is available
        virtual void NotifyVisionResult(int requestId, const std::vector<std::string>& results) = 0;
    };
}
