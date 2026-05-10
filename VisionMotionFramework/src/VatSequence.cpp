#include "StdAfx.h"

#include "VatSequence.h"
#include "ITask.h"
#include "VAT_Context.h"
#include <iostream>
#include <exception>
#include <sstream>
#include <mutex>
#include <chrono>

namespace DVH_VAT
{
    VatSequence::VatSequence(const std::string& name)
        : m_abortRequested(false)
        , m_pollIntervalMs(10)
        , m_SequenceName(name)
    {
        LogTask(makeLogPrefix(m_SequenceName) + "constructed");
    }

    // 소멸자는 헤더에서 = default로 선언되었으므로 생략 가능하지만, 
    // 기존에 소멸 로그가 존재하므로 로직을 헤더에서 옮겨오거나 계속 유지합니다.
    VatSequence::~VatSequence()
    {
        LogTask(makeLogPrefix(m_SequenceName) + "destructed");
    }

    void VatSequence::AddTask(TaskStepPtr task)
    {
        std::lock_guard<std::mutex> lock(m_mutex); // boost::mutex::scoped_lock -> std::lock_guard
    
        m_tasks.push_back(task);
        LogTask(makeLogPrefix(m_SequenceName) + "AddTask: " + (task ? task->GetName() : "<null>"));
    }

    std::string VatSequence::GetSequenceName() const
    {
        return m_SequenceName;
    }

    std::string VatSequence::GetTaskName() const
    {
        return m_TaskName;
    }
    
    void VatSequence::Abort()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_abortRequested = true; // 이제 std::atomic이므로 락 없이도 스레드 안전하지만 cv를 위해 락 유지
        
        LogTask(makeLogPrefix(m_SequenceName) + "Abort requested");
        m_cv.notify_all();
    }

    bool VatSequence::Execute(VAT_Context& context, IVatActuator* actuator)
    {
        LogTask(makeLogPrefix(m_SequenceName) + "Execute start");

        // 초기화
        {
             std::lock_guard<std::mutex> lock(m_mutex);
             m_abortRequested = false;
        }

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
                 LogTask(makeLogPrefix(m_SequenceName) + "Stop requested - aborting sequence");
                 return false;
            }

            TaskStepPtr curTask;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (idx >= m_tasks.size())
                {
                     // 모든 스텝 완료
                     LogTask(makeLogPrefix(m_SequenceName) + "All steps finished.");
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
                res = curTask->Execute(context, actuator);
            }
            catch (const std::exception& ex)
            {
                LogTask(makeLogPrefix(m_SequenceName) + "Exception in step->Execute: " + ex.what());
                return false;
            } catch (...)
            {
                LogTask(makeLogPrefix(m_SequenceName) + "Unknown exception in step->Execute");
                return false;
            }

            if (res != TR_KEEP) {
                 // C++11 std::to_string 활용
                 LogTask(makeLogPrefix(m_SequenceName) + "Step [" + curTask->GetName() + "] returned: " + std::to_string(static_cast<int>(res)));
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

} // namespace DVH_VAT

