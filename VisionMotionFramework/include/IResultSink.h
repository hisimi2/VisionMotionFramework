#pragma once

#include <vector>
#include <string>

namespace VMF
{
    class IResultSink
    {
    public:
        virtual ~IResultSink() = default;

        // called when a new result is available
        virtual void NotifyVisionResult(int requestId, const std::vector<std::string>& results) = 0;
    };
}

