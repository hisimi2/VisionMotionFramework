#include "StdAfx.h"
#include "Activity.h"

#include "Utils.h"
#include "ITask.h"
#include "Context.h"

#include <exception>
#include <mutex>
#include <chrono>
#include <thread>

namespace EC
{       
    Activity::Activity(const std::string& name)
        : m_pollIntervalMs(10)
        , m_ActivityName(name)
    {
        LogTask(makeLogPrefix(m_ActivityName) + "Activity constructed");
    }

    Activity::~Activity()
    {
        LogTask(makeLogPrefix(m_ActivityName) + "Activity destructed");
    }

    void Activity::AddTask(TaskPtr task)
    {
        std::lock_guard<std::mutex> lock(m_taskMutex);
        m_tasks.push_back(task);
        LogTask(makeLogPrefix(m_ActivityName) + "AddTask: " + (task ? task->GetName() : "<null>"));
    }

    std::string Activity::GetActivityName() const
    {
        return m_ActivityName;
    }

    bool Activity::Execute(Context& ctx)
    {
        LogTask(makeLogPrefix(m_ActivityName) + "Execute start");

        // Task 목록 스냅샷 (lock 최소화)
        std::vector<TaskPtr> tasks;
        {
            std::lock_guard<std::mutex> lock(m_taskMutex);
            tasks = m_tasks;
        }

        if (tasks.empty())
        {
            LogTask(makeLogPrefix(m_ActivityName) + "No tasks registered");
            return true;
        }

        size_t idx = 0;

        while (true)
        {
            // 중단 요청 체크 (Context 기반 — 단일 중단 메커니즘)
            if (ctx.isStop())
            {
                LogTask(makeLogPrefix(m_ActivityName) + "Stop requested - aborting Activity");
                return false;
            }

            // 모든 Task 완료
            if (idx >= tasks.size())
            {
                LogTask(makeLogPrefix(m_ActivityName) + "All steps finished.");
                return true;
            }

            TaskPtr curTask = tasks[idx];
            if (!curTask)
            {
                LogTask(makeLogPrefix(m_ActivityName) + "Null task at index " + std::to_string(idx) + " - skipping");
                ++idx;
                continue;
            }

            TaskResult res = TR_ERROR;
            try
            {
                if (ctx.isResume())
                {
                    res = curTask->Execute(ctx);
                }
                else
                {
                    res = TR_KEEP;
                }
            }
            catch (const std::exception& ex)
            {
                LogTask(makeLogPrefix(m_ActivityName) + "Exception in step [" + curTask->GetName() + "] Execute: " + ex.what());
                ctx.SetLastError(std::string("Activity exception: ") + ex.what());
                return false;
            }
            catch (...)
            {
                LogTask(makeLogPrefix(m_ActivityName) + "Unknown exception in step [" + curTask->GetName() + "] Execute");
                ctx.SetLastError("Activity: unknown exception");
                return false;
            }

            LogTask(makeLogPrefix(m_ActivityName) + "Step [" + curTask->GetName() + "] returned: " + std::to_string(static_cast<int>(res)));

            switch (res)
            {
            case TR_KEEP:
                std::this_thread::sleep_for(std::chrono::milliseconds(m_pollIntervalMs));
                break;

            case TR_NEXT:
                ++idx;
                break;

            case TR_PREV:
                if (idx > 0) --idx;
                else LogTask(makeLogPrefix(m_ActivityName) + "Can't Move To Prev Step. Current is First Step");
                break;

            case TR_DONE:
                LogTask(makeLogPrefix(m_ActivityName) + "Activity signaled DONE by step [" + curTask->GetName() + "]");
                return true;

            case TR_ERROR:
                LogTask(makeLogPrefix(m_ActivityName) + "Activity signaled ERROR by step [" + curTask->GetName() + "]");
                return false;

            default:
                LogTask(makeLogPrefix(m_ActivityName) + "Step [" + curTask->GetName() + "] returned unknown result: " + std::to_string(static_cast<int>(res)));
                return false;
            }
        }
    }
} 

