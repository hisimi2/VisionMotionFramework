#pragma once

#include "ITask.h"
#include "Context.h"
#include "IActuator.h"

#include <mutex>
#include <chrono>
#include <string>
#include <exception>

namespace VMF
{
/// <summary>
    /// 비동기(비차단) 작업 스텝의 공통 기초 클래스입니다.
    /// 각 스텝은 상태를 가지며 OnInitialize / OnPoll 패턴으로 실행됩니다.
    /// 스레드 안전을 위해 내부적으로 std::mutex를 사용하며, 타임아웃(데드라인) 기능을 제공합니다.
    /// 
/// [Context 기반 파라미터]
    /// Task는 Context를 통해 파라미터를 전달받습니다.
    /// SequenceBuilder가 시퀀스 실행 전 Context에 파라미터를 설정하고,
    /// Task는 provider.GetTaskParams().GetExecutionParam<T>()로 조회합니다.
    /// 
    /// 사용 예:
    ///   void OnInitialize(VMF::Context& ctx) override {
    ///       const auto& provider = static_cast<const VMF::IParamProvider&>(ctx);
    ///       m_timeoutMs = provider.GetTaskParams().GetExecutionParam<int>("Setup.TIMEOUT_MOVE_MS", 5000);
    ///   }
    /// </summary>
    class NonBlockingTaskBase : public ITask
    {
    public:

        static const int CS_INITIALIZING    = -1; // 초기화 상태 (공통)
        static const int CS_ERROR           = -2; // 에러 상태 (공통)
        static const int CS_IDLE            = -3; // 대기 상태 (공통)

        /// <summary>
        /// 기본 생성자. 내부 상태와 플래그를 초기화합니다.
        /// </summary>
        NonBlockingTaskBase()
            : m_state_(CS_INITIALIZING)
            , m_initialized_(false)
            , m_hasDeadline_(false)
        {
        }

        /// <summary>
        /// 가상 소멸자. 파생 클래스에서 안전하게 파괴될 수 있도록 합니다.
        /// </summary>
        ~NonBlockingTaskBase() override = default;

        /// <summary>
        /// 스텝 실행 진입점입니다.
        /// - Stop 요청이 있으면 에러로 종료합니다.
        /// - 최초 호출 시 OnInitialize를 호출합니다.
        /// - 이후에는 OnPoll을 호출하여 상태를 진행합니다.
        /// 예외는 캐치되어 컨텍스트에 마지막 오류로 기록되고 TR_ERROR를 반환합니다.
        /// </summary>
        /// <param name="ctx">공유 실행 컨텍스트</param>
        /// <param name="actuator">Actuator 인터페이스 (존재하지 않을 수 있음)</param>
        /// <returns>TaskResult 값</returns>
        TaskResult Execute(Context& ctx, IActuator* actuator) override
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
                return OnPoll(ctx, actuator);
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

        /// <summary>
        /// 현재 상태를 반환합니다.
        /// </summary>
        int GetState() const
        {
            return m_state_;
        }

        /// <summary>
        /// 지정한 상태로 전이하고 타임아웃(데드라인)을 설정합니다.
        /// </summary>
        /// <param name="newState">진입할 상태 식별자</param>
        /// <param name="timeoutMs">타임아웃 시간(밀리초)</param>
        void EnterStateWithTimeout(int newState, long timeoutMs)
        {
            m_state_ = newState;
            m_hasDeadline_ = true;
            m_deadline_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
        }

        /// <summary>
        /// 설정된 데드라인(타임아웃)이 만료되었는지 확인합니다.
        /// </summary>
        bool IsDeadlineExpired() const
        {
            if (!m_hasDeadline_)
                return false;
            return std::chrono::steady_clock::now() > m_deadline_;
        }

private:
        int                                   m_state_;
        bool                                  m_initialized_;
        bool                                  m_hasDeadline_;
        
        mutable std::mutex                    m_mutex_;
        std::chrono::steady_clock::time_point m_deadline_;
    };
}
