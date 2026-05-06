#pragma once
#include <cstddef>
#include <boost/function.hpp>

#include "IScheduler.h"

namespace VisionCom
{
    typedef boost::function<void()> WorkTask;

    class WorkerScheduler : public IScheduler
    {
    public:
        explicit WorkerScheduler(size_t threadCount = 1);
        virtual ~WorkerScheduler();

        virtual void Schedule(const SchedulerTask& task, int delayMs = 0);
        virtual void Start();
        virtual void Stop();

        void Enqueue(WorkTask t);   

    private:
        struct Impl;
        Impl* m_pImpl;

        WorkerScheduler(const WorkerScheduler&);
        WorkerScheduler& operator=(const WorkerScheduler&) { return *this; }
    };
} 
