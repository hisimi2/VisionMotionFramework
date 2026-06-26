#include "StdAfx.h"

#include "Sequence.h"
#include "ITask.h"
#include "Context.h"
#include <mutex>
#include <chrono>

namespace VMF
{
    Sequence::Sequence(const std::string& name)
        : m_abortRequested(false)
        , m_pollIntervalMs(10)
        , m_sequenceName(name)
    {
    }

    Sequence::~Sequence()
    {
    }

    void Sequence::AddTask(TaskPtr task)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.push_back(task);
    }

    std::string Sequence::GetSequenceName() const
    {
        return m_sequenceName;
    }

    std::string Sequence::GetTaskName() const
    {
        return m_taskName;
    }
    
    void Sequence::Abort()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_abortRequested = true;
        m_cv.notify_all();
    }

    bool Sequence::Execute(Context& context, IActuator* actuator)
    {
        // 초기화
        {
             std::lock_guard<std::mutex> lock(m_mutex);
             m_abortRequested = false;
        }

        size_t idx = 0;

        while (true)
        {
            // 중단 요청 체크 (컨텍스트 또는 자체 요청)
            if (context.GetStopRequested())
            {
                 return false;
            }

            {
                 std::lock_guard<std::mutex> lock(m_mutex);
                 if (m_abortRequested) return false;
            }

            TaskPtr curTask;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (idx >= m_tasks.size())
                {
                     return true; 
                }
                curTask = m_tasks[idx];
            }

            if (!curTask) { 
                ++idx; 
                continue; 
            }

            TaskResult res = TR_ERROR;
            try
            {
                m_taskName = curTask->GetName();
                res = curTask->Execute(context, actuator);
            }
            catch (const std::exception&)
            {
                return false;
            } catch (...)
            {
                return false;
            }

            switch (res)
            {
            case TR_KEEP:
            {
                std::unique_lock<std::mutex> lock(m_mutex); 
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
                break;

            case TR_DONE:
                return true;

            case TR_ERROR:
                return false;

            default:
                return false;
            }
        }
    }

} // namespace VMF

