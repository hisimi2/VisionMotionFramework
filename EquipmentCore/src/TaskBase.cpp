#include "stdafx.h"
#include "TaskBase.h"

namespace EC
{
    TaskBase::TaskBase(std::string name)
        : m_step_(CS_INITIALIZING)
        , m_initialized_(false)
        , m_hasDeadline_(false)
        , m_name(std::move(name))
    {
    }

    TaskResult TaskBase::Execute(Context& ctx)
    {
        // Stop 요청 체크 (lock 외부에서 먼저 확인)
        if (ctx.GetStopRequested())
        {
            ctx.SetLastError("TaskBase: Stop requested");
            m_step_ = CS_ERROR;
            return TR_ERROR;
        }

        // --- 1회 초기화 (lock 외부에서 상태만 확인, 초기화 자체는 내부에서 처리) ---
        if (!m_initialized_)
        {
            m_initialized_ = true;
            try
            {
                OnInitialize(ctx);
            }
            catch (const std::exception& ex)
            {
                ctx.SetLastError(std::string("TaskBase: exception in OnInitialize: ") + ex.what());
                m_step_ = CS_ERROR;
                return TR_ERROR;
            }
            catch (...)
            {
                ctx.SetLastError("TaskBase: unknown exception in OnInitialize");
                m_step_ = CS_ERROR;
                return TR_ERROR;
            }
        }

        // --- 상태가 에러이면 바로 반환 ---
        if (m_step_ == CS_ERROR)
        {
            return TR_ERROR;
        }

        // --- Poll 실행 ---
        try
        {
            return OnPoll(ctx);
        }
        catch (const std::exception& ex)
        {
            ctx.SetLastError(std::string("TaskBase: exception in OnPoll: ") + ex.what());
            m_step_ = CS_ERROR;
            return TR_ERROR;
        }
        catch (...)
        {
            ctx.SetLastError("TaskBase: unknown exception in OnPoll");
            m_step_ = CS_ERROR;
            return TR_ERROR;
        }
    }

    void TaskBase::Abort()
    {
        m_step_ = CS_ERROR;
    }

    void TaskBase::EnterState(int newState)
    {
        m_step_ = newState;
        m_hasDeadline_ = false;
    }

    std::string TaskBase::GetName() const
    {
        return m_name;
    }

    std::mutex& TaskBase::GetMutex()
    {
        return m_mutex_;
    }

    TaskResult TaskBase::SetErrorAndReturn(Context& ctx, const std::string& msg)
    {
        ctx.SetLastError(msg);
        EnterState(CS_ERROR);
        return TR_ERROR;
    }

    void TaskBase::EnterStateWithTimeout(int newState, long timeoutMs)
    {
        m_step_ = newState;

        if (timeoutMs <= 0)
        {
            m_hasDeadline_ = false;
        }
        else
        {
            m_deadline_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
            m_hasDeadline_ = true;
        }
    }

    int TaskBase::GetState() const
    {
        return m_step_;
    }

    bool TaskBase::IsDeadlineExpired() const
    {
        if (!m_hasDeadline_)
        {
            return false;
        }

        return std::chrono::steady_clock::now() >= m_deadline_;
    }
}
