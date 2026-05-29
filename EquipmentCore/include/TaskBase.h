#pragma once

#include <mutex>
#include <chrono>
#include <string>
#include <exception>
#include "Context.h"

#include "EC_API.h"
#include "ITask.h"

namespace EC
{
    class EC_API TaskBase : public ITask
    {
    public:

        static const int CS_INITIALIZING = -1;  // 초기화 상태 (공통)
        static const int CS_ERROR = -2;         // 에러 상태 (공통)
        static const int CS_IDLE = -3;          // 대기 상태 (공통)
        TaskBase()
            : m_state_(CS_INITIALIZING)
            , m_initialized_(false)
            , m_hasDeadline_(false)
        {
        }
        ~TaskBase() override = default;

        TaskResult Execute(Context& ctx) override
        {
            std::lock_guard<std::mutex> lg(m_mutex_);

            if (ctx.GetStopRequested())
            {
                ctx.SetLastError("NonBlockingTaskBase: Stop requested");
                m_state_ = CS_ERROR;
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
                    ctx.SetLastError(std::string("NonBlockingTaskBase: exception in OnInitialize: ") + ex.what());
                    m_state_ = CS_ERROR;
                    return TR_ERROR;
                }
                catch (...)
                {
                    ctx.SetLastError("NonBlockingTaskBase: unknown exception in OnInitialize");
                    m_state_ = CS_ERROR;
                    return TR_ERROR;
                }
            }

            try
            {
                return OnPoll(ctx);
            }
            catch (const std::exception& ex)
            {
                ctx.SetLastError(std::string("NonBlockingTaskBase: exception in OnPoll: ") + ex.what());
                m_state_ = CS_ERROR;
                return TR_ERROR;
            }
            catch (...)
            {
                ctx.SetLastError("NonBlockingTaskBase: unknown exception in OnPoll");
                m_state_ = CS_ERROR;
                return TR_ERROR;
            }
        }

        /// <summary>
        /// 강제 중단을 요청합니다. 내부 상태를 에러 상태로 전환합니다.
        /// </summary>
        void Abort() override
        {
            std::lock_guard<std::mutex> lg(m_mutex_);
            m_state_ = CS_ERROR;
        }

        /// <summary>
        /// 지정한 상태로 진입합니다. 데드라인(타임아웃)은 초기화됩니다.
        /// </summary>
        /// <param name="newState">진입할 상태 식별자</param>
        void EnterState(int newState) override
        {
            m_state_ = newState;
            m_hasDeadline_ = false;
        }

        /// <summary>
        /// 스텝의 이름을 반환합니다. 파생 클래스에서 구현해야 합니다.
        /// </summary>
        std::string GetName() const override = 0;

        /// <summary>
        /// 외부에서 이 스텝의 뮤텍스를 획득할 수 있게 합니다.
        /// 스레드 동기화를 위해 필요할 때 사용합니다.
        /// </summary>
        /// <returns>내부 뮤텍스 참조</returns>
        std::mutex& GetMutex()
        {
            return m_mutex_;
        }

        /// <summary>
        /// 오류 메시지를 컨텍스트에 기록하고 지정한 상태로 전이한 후 TR_ERROR를 반환합니다.
        /// 파생 클래스에서 에러 처리 및 반환을 간단히 하기 위한 헬퍼입니다.
        /// </summary>
        TaskResult SetErrorAndReturn(Context& ctx, const std::string& msg) override
        {
            ctx.SetLastError(msg);
            EnterState(CS_ERROR);
            return TR_ERROR;
        }

    protected:
        /// <summary>
        /// 스텝 초기화 시 호출되는 콜백입니다. 파생 클래스에서 초기화 로직을 구현합니다.
        /// (Execute에서 최초 한 번 호출됨)
        /// </summary>
        virtual void OnInitialize(Context& ctx) = 0;

        /// <summary>
        /// 스텝 진행 시 반복적으로 호출되는 폴링 콜백입니다.
        /// 상태 전이 및 작업 완료 판정을 이곳에서 수행합니다.
        /// </summary>
        /// <returns>다음 동작을 나타내는 TaskResult</returns>
        virtual TaskResult OnPoll(Context& ctx) = 0;

        /// <summary>
        /// 공통 상태(CBS_*)로 진입합니다. 데드라인은 초기화됩니다.
        /// </summary>
        /// <param name="state">진입할 공통 상태</param>
        void EnterCommonState(int state)
        {
            m_state_ = state;
            m_hasDeadline_ = false;
        }

        /// <summary>
        /// 지정한 상태로 진입하고 타임아웃(밀리초)을 설정합니다.
        /// timeoutMs <= 0 이면 데드라인을 사용하지 않습니다.
        /// </summary>
        /// <param name="newState">진입할 상태</param>
        /// <param name="timeoutMs">데드라인까지의 시간(밀리초)</param>
        void EnterStateWithTimeout(int newState, long timeoutMs)
        {
            m_state_ = newState;

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
        int GetState() const
        {
            return m_state_;
        }

        /// <summary>
        /// 설정된 데드라인이 만료되었는지 검사합니다.
        /// 데드라인이 설정되지 않은 경우 false를 반환합니다.
        /// </summary>
        /// <returns>만료되었으면 true, 아니면 false</returns>
        bool IsDeadlineExpired() const
        {
            if (!m_hasDeadline_)
            {
                return false;
            }

            return std::chrono::steady_clock::now() >= m_deadline_;
        }

    private:
        int                                   m_state_;
        bool                                  m_initialized_;
        bool                                  m_hasDeadline_;

        // 동기화 및 시간 관리 멤버를 C++ 표준 라이브러리로 대체
        mutable std::mutex                    m_mutex_;
        std::chrono::steady_clock::time_point m_deadline_;
    };
}
