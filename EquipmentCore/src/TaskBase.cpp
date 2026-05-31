#include "stdafx.h"
#include "TaskBase.h"

namespace EC
{
    TaskBase::TaskBase(std::string name)
        : m_step_(CS_INITIALIZING)
        , m_initialized_(false)
        , m_hasDeadline_(false)
        , m_name(name)
    {
    }

    TaskResult TaskBase::Execute(Context& ctx)
    {
        std::lock_guard<std::mutex> lg(m_mutex_);

        if (ctx.GetStopRequested())
        {
            ctx.SetLastError("TaskBase: Stop requested");
            m_step_ = CS_ERROR;
            return TR_ERROR;
        }

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

    /// <summary>
    /// 강제 중단을 요청합니다. 내부 상태를 에러 상태로 전환합니다.
    /// </summary>
    void TaskBase::Abort()
    {
        std::lock_guard<std::mutex> lg(m_mutex_);
        m_step_ = CS_ERROR;
    }

    /// <summary>
    /// 지정한 상태로 진입합니다. 데드라인(타임아웃)은 초기화됩니다.
    /// </summary>
    /// <param name="newState">진입할 상태 식별자</param>
    void TaskBase::EnterState(int newState)
    {
        m_step_ = newState;
        m_hasDeadline_ = false;
    }

    /// <summary>
    /// 스텝의 이름을 반환합니다. 파생 클래스에서 구현해야 합니다.
    /// </summary>
    std::string TaskBase::GetName() const
    {
        return m_name;
    }

    /// <summary>
    /// 외부에서 이 스텝의 뮤텍스를 획득할 수 있게 합니다.
    /// 스레드 동기화를 위해 필요할 때 사용합니다.
    /// </summary>
    /// <returns>내부 뮤텍스 참조</returns>
    std::mutex& TaskBase::GetMutex()
    {
        return m_mutex_;
    }

    /// <summary>
    /// 오류 메시지를 컨텍스트에 기록하고 지정한 상태로 전이한 후 TR_ERROR를 반환합니다.
    /// 파생 클래스에서 에러 처리 및 반환을 간단히 하기 위한 헬퍼입니다.
    /// </summary>
    TaskResult TaskBase::SetErrorAndReturn(Context& ctx, const std::string& msg)
    {
        ctx.SetLastError(msg);
        EnterState(CS_ERROR);
        return TR_ERROR;
    }

    /// <summary>
    /// 지정한 상태로 진입하고 타임아웃(밀리초)을 설정합니다.
    /// timeoutMs <= 0 이면 데드라인을 사용하지 않습니다.
    /// </summary>
    /// <param name="newState">진입할 상태</param>
    /// <param name="timeoutMs">데드라인까지의 시간(밀리초)</param>
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

    /// <summary>
    /// 현재 상태 식별자를 반환합니다.
    /// </summary>
    /// <returns>현재 상태 값</returns>
    int TaskBase::GetState() const
    {
        return m_step_;
    }

    /// <summary>
    /// 설정된 데드라인이 만료되었는지 검사합니다.
    /// 데드라인이 설정되지 않은 경우 false를 반환합니다.
    /// </summary>
    /// <returns>만료되었으면 true, 아니면 false</returns>
    bool TaskBase::IsDeadlineExpired() const
    {
        if (!m_hasDeadline_)
        {
            return false;
        }

        return std::chrono::steady_clock::now() >= m_deadline_;
    }
}
