#include "StdAfx.h"

#include "VatSequence.h"
#include "ITask.h"
#include "VAT_Context.h"
#include <iostream>
#include <exception>
#include <sstream>

namespace DVH_VAT
{
    VatSequence::VatSequence(const std::string& name)
        : m_abortRequested(false)
        , m_pollIntervalMs(10)
        , m_SequenceName(name)
    {
        LogTask(makeLogPrefix(m_SequenceName) + "constructed");
    }

    VatSequence::~VatSequence()
    {
        LogTask(makeLogPrefix(m_SequenceName) + "destructed");
    }

    void VatSequence::AddTask(TaskStepPtr task)
    {
        boost::mutex::scoped_lock lock(m_mutex);
    
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
        boost::mutex::scoped_lock lock(m_mutex);
        m_abortRequested = true;
        
        LogTask(makeLogPrefix(m_SequenceName) + "Abort requested");
        m_cv.notify_all();
    }

    bool VatSequence::Execute(VAT_Context& context, IVatActuator* actuator)
    {
        LogTask(makeLogPrefix(m_SequenceName) + "Execute start");

        // 초기화
        {
             boost::mutex::scoped_lock lock(m_mutex);
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
                 boost::mutex::scoped_lock lock(m_mutex);
                 if (m_abortRequested) stopReq = true;
            }

            if (stopReq)
            {
                 LogTask(makeLogPrefix(m_SequenceName) + "Stop requested - aborting sequence");
                 return false;
            }

            TaskStepPtr curTask;
            {
                boost::mutex::scoped_lock lock(m_mutex);
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
                 // 상태 변화가 있을 때만 로그 출력
                 std::ostringstream oss; 
                 oss << "Step [" << curTask->GetName() << "] returned: " << (int)res;
                 LogTask(makeLogPrefix(m_SequenceName) + oss.str());
            }

            switch (res)
            {
            case TR_KEEP:
            {
                boost::mutex::scoped_lock lock(m_mutex);
            
                // Abort나 Stop 요청이 오면 즉시 깨어남, 아니면 m_pollIntervalMs 만큼 대기
                // wait_for의 predicate 버전 대신 loop 사용
                if (!m_abortRequested && !context.GetStopRequested())
                {
                    m_cv.wait_for(lock, boost::chrono::milliseconds(m_pollIntervalMs));
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

