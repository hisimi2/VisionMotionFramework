#include "StdAfx.h"
#include "Sequence.h"

#include "Utils.h"
#include "ITask.h"
#include "Context.h"

#include <iostream>
#include <exception>
#include <sstream>
#include <mutex>
#include <chrono>

namespace EC
{
    Sequence::Sequence(const std::string& name)
        : m_abortRequested(false)
        , m_pollIntervalMs(10)
        , m_SequenceName(name)
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
        return m_SequenceName;
    }

    std::string Sequence::GetTaskName() const
    {
        return m_TaskName;
    }
    
    void Sequence::Abort()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_abortRequested = true;
        m_cv.notify_all();

    }

    bool Sequence::Execute(Context& context)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_abortRequested = false;

        size_t idx = 0;
    
        // 무한 루프 방지용 최대 스텝 수 제한이 필요하다면 추가, 여기선 while(true) 유지
        while (true)
        {
            // 중단 요청 체크 (컨텍스트 또는 자체 요청)
            bool stopReq = context.GetStopRequested();
            if (!stopReq)
            {
                 // m_abortRequested가 std::atomic<bool>이므로 락 불필요해 읽을 수 있지만 일관성을 위해 유지
                 std::lock_guard<std::mutex> lock(m_mutex);
                 if (m_abortRequested) stopReq = true;
            }

            if (stopReq)
            {
                 return false;
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
                m_TaskName = curTask->GetName();
                res = curTask->Execute(context);
            }
            catch (const std::exception& ex)
            {
                return false;
            } catch (...)
            {
                return false;
            }

            if (res != TR_KEEP)
            {
            }

            switch (res)
            {
            case TR_KEEP:
            {
                // std::condition_variable은 std::unique_lock을 요구합니다.
                std::unique_lock<std::mutex> lock(m_mutex); 
            
                // Abort나 Stop 요청이 오면 즉시 깨어남, 아니면 m_pollIntervalMs 만큼 대기
                if (!m_abortRequested && !context.GetStopRequested())
                {
                    // boost::chrono -> std::chrono 교체
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
                    LogTask(makeLogPrefix(m_SequenceName) + "Can't Move To Prev Step. Current is First Step");
                break;

            case TR_DONE:
                LogTask(makeLogPrefix(m_SequenceName) + "Sequence signaled DONE by step");
                // 전체 시퀀스 완료로 간주하려면 true 리턴, 
                // 현재 스텝만 완료라면 ++idx (여기서는 시퀀스 전체 완료 의미로 해석)
                return true;

            case TR_ERROR:
                LogTask(makeLogPrefix(m_SequenceName) + "Sequence signaled ERROR by step");
                return false;

            default:
                LogTask(makeLogPrefix(m_SequenceName) + "Step returned unknown result");
                return false;
            }
        }

        LogTask(makeLogPrefix(m_SequenceName) + "Execute finished normally");
        return true;
    }

} // namespace EC

