#include "StdAfx.h"
#include "Activity.h"

#include "Utils.h"
#include "ITask.h"
#include "Context.h"

#include <exception>
#include <mutex>
#include <chrono>

namespace EC
{
    Activity::Activity(const std::string& name)
        : m_abortRequested(false)
        , m_pollIntervalMs(10)
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
        std::lock_guard<std::mutex> lock(m_mutex); 
        m_tasks.push_back(task);
        LogTask(makeLogPrefix(m_ActivityName) + "AddTask: " + (task ? task->GetName() : "<null>"));
    }

    std::string Activity::GetActivityName() const
    {
        return m_ActivityName;
    }

    std::string Activity::GetTaskName() const
    {
        if(m_curTask)
            return m_curTask->GetName();
        else
            return "";
    }
    
    void Activity::Abort()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_abortRequested = true;
        }
        LogTask(makeLogPrefix(m_ActivityName) + "Abort requested");
        // 락을 해제한 후 notify -> 대기 중인 스레드가 즉시 락을 획득 가능
        m_cv.notify_all();
    }

    bool Activity::Execute(Context& context)
    {
        // lock_guard 대신 unique_lock 사용 (TR_KEEP에서 wait_for를 위해 unlock/relock 필요)
        std::unique_lock<std::mutex> lock(m_mutex);
        m_abortRequested = false;

        LogTask(makeLogPrefix(m_ActivityName) + "Execute start");

        size_t idx = 0;
    
        while (true)
        {
            // 중단 요청 체크 (컨텍스트 또는 자체 요청) - 이미 락 획득 상태
            if (context.GetStopRequested())
            {
                LogTask(makeLogPrefix(m_ActivityName) + "Stop requested - aborting Activity");
                return false;
            }

            if (m_abortRequested)
            {
                LogTask(makeLogPrefix(m_ActivityName) + "Abort requested - aborting Activity");
                return false;
            }

            // Task 목록 접근 - 이미 락 획득 상태
            if (idx >= m_tasks.size())
            {
                LogTask(makeLogPrefix(m_ActivityName) + "All steps finished.");
                return true;
            }

            m_curTask = m_tasks[idx];
            if (!m_curTask)
            {
                LogTask(makeLogPrefix(m_ActivityName) + "Null task at index " + std::to_string(idx) + " - skipping");
                ++idx;
                continue;
            }

            // Task 실행 중에는 뮤텍스를 해제하여 다른 스레드가 Abort()를 호출할 수 있게 함
            lock.unlock();

            TaskResult res = TR_ERROR;
            try
            {
                res = m_curTask->Execute(context);
            }
            catch (const std::exception& ex)
            {
                LogTask(makeLogPrefix(m_ActivityName) + "Exception in step [" + m_curTask->GetName() + "] Execute: " + ex.what());
                return false;
            }
            catch (...)
            {
                LogTask(makeLogPrefix(m_ActivityName) + "Unknown exception in step [" + m_curTask->GetName() + "] Execute");
                return false;
            }

            // 결과 처리를 위해 뮤텍스 재획득
            lock.lock();

            LogTask(makeLogPrefix(m_ActivityName) + "Step [" + m_curTask->GetName() + "] returned: " + std::to_string(static_cast<int>(res)));

            switch (res)
            {
            case TR_KEEP:
            {
                // wait_for는 내부에서 unlock -> 대기 -> relock 수행
                // notify나 timeout으로 깨어남
                if (!m_abortRequested && !context.GetStopRequested())
                {
                    m_cv.wait_for(lock, std::chrono::milliseconds(m_pollIntervalMs));
                }
                break;
            }

            case TR_NEXT:
                ++idx;
                break;

            case TR_PREV:
                if (idx > 0)
                    --idx;
                else
                    LogTask(makeLogPrefix(m_ActivityName) + "Can't Move To Prev Step. Current is First Step");
                break;

            case TR_DONE:
                LogTask(makeLogPrefix(m_ActivityName) + "Activity signaled DONE by step [" + m_curTask->GetName() + "]");
                return true;

            case TR_ERROR:
                LogTask(makeLogPrefix(m_ActivityName) + "Activity signaled ERROR by step [" + m_curTask->GetName() + "]");
                return false;

            default:
                LogTask(makeLogPrefix(m_ActivityName) + "Step [" + m_curTask->GetName() + "] returned unknown result: " + std::to_string(static_cast<int>(res)));
                return false;
            }
        }
    }

} // namespace EC

