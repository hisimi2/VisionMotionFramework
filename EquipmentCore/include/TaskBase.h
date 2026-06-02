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

        static const int CS_INITIALIZING    = -1;  // 초기화 상태 (공통)
        static const int CS_ERROR           = -2;  // 에러 상태 (공통)
        static const int CS_ABORT           = -3;  // 강제 중단 상태 (공통)

        TaskBase(std::string name);
        ~TaskBase() override = default;

        TaskResult Execute(Context& ctx) override;
        
        /// <summary>
        /// 강제 중단을 요청합니다. 내부 상태를 에러 상태로 전환합니다.
        /// </summary>
        void Abort();

        /// <summary>
        /// 지정한 상태로 진입합니다. 데드라인(타임아웃)은 초기화됩니다.
        /// </summary>
        void EnterState(int newState);

        /// <summary>
        /// 스텝의 이름을 반환합니다.
        /// </summary>
        std::string GetName() const override;

        /// <summary>
        /// 오류 메시지를 컨텍스트에 기록하고 CS_ERROR 상태로 전이한 후 TR_ERROR를 반환합니다.
        /// </summary>
        TaskResult SetErrorAndReturn(Context& ctx, const std::string& msg);

    protected:
        /// <summary>
        /// 스텝 초기화 시 호출되는 콜백. Execute()에서 최초 한 번 호출됩니다.
        /// </summary>
        virtual void OnInitialize(Context& ctx) = 0;

        /// <summary>
        /// 스텝 진행 시 반복적으로 호출되는 폴링 콜백.
        /// 상태 전이 및 작업 완료 판정을 수행합니다.
        /// </summary>
        virtual TaskResult OnPoll(Context& ctx) = 0;

        /// <summary>
        /// 지정한 상태로 진입하고 타임아웃(밀리초)을 설정합니다.
        /// timeoutMs <= 0 이면 데드라인을 사용하지 않습니다.
        /// </summary>
        void EnterStateWithTimeout(int newState, long timeoutMs);

        /// <summary>
        /// 현재 상태 식별자를 반환합니다.
        /// </summary>
        int GetStep() const;

        /// <summary>
        /// 설정된 데드라인이 만료되었는지 검사합니다.
        /// </summary>
        bool IsDeadlineExpired() const;

    private:
        int                                     m_step_;
        bool                                    m_initialized_;
        bool                                    m_hasDeadline_;

        std::chrono::steady_clock::time_point   m_deadline_;
        std::string                             m_name;
    };
}
