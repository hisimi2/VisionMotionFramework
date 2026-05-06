#pragma once

#include <boost/function.hpp>

namespace VisionCom
{
    typedef boost::function<void()> SchedulerTask; 

    class IScheduler 
    {
    public:
        virtual ~IScheduler() {}

        virtual void Schedule(const SchedulerTask& task, int delayMs = 0) = 0;

        virtual void Start() = 0;
        virtual void Stop() = 0;
    };

}
