#pragma once

#include <functional> 

namespace VC
{
    using SchedulerTask = std::function<void()>;

    class IScheduler
    {
    public:
        virtual ~IScheduler() = default;
        virtual void Schedule(const SchedulerTask& task, int delayMs = 0) = 0;
        virtual void Start() = 0;
        virtual void Stop() = 0;
    };

} // namespace VCm

