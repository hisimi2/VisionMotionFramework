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
        // --- 1회 초기화 ---
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

        // 에러 상태면 바로 반환
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

    int TaskBase::GetStep() const
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
