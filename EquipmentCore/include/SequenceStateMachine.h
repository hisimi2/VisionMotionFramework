#pragma once

#include <mutex>
#include <condition_variable>
#include <memory>
#include "EC_API.h"

namespace EC
{
    /// <summary>
    /// 시퀀스 실행 상태를 관리하는 상태머신
    /// Run(순차진행) -> Stop(대기) -> Run -> Exit(완전종료)
    /// </summary>
    class EC_API SequenceStateMachine
    {
    public:
        /// <summary>
        /// 시퀀스 실행 상태
        /// </summary>
        enum class SequenceState
        {
            Run,    // 시퀀스 순차 진행 상태
            Stop,   // 시퀀스 진행 대기 상태
            Exit    // 시퀀스 완전 종료 상태
        };

        explicit SequenceStateMachine();
        ~SequenceStateMachine();

        /// <summary>
        /// 현재 상태를 Run으로 전환하여 시퀀스 진행을 시작합니다.
        /// </summary>
        void Start();

        /// <summary>
        /// 현재 상태를 Stop으로 전환하여 시퀀스 진행을 일시 대기합니다.
        /// </summary>
        void Pause();

        /// <summary>
        /// 현재 상태를 Exit으로 전환하여 시퀀스 완전 종료를 요청합니다.
        /// </summary>
        void Terminate();

        /// <summary>
        /// 현재 상태를 반환합니다.
        /// </summary>
        /// <returns>현재 SequenceState</returns>
        SequenceState GetCurrentState() const;

        /// <summary>
        /// 현재 상태가 Run 상태인지 확인합니다.
        /// </summary>
        /// <returns>Run 상태면 true, 아니면 false</returns>
        bool IsRunning() const;

        /// <summary>
        /// 현재 상태가 Stop 상태인지 확인합니다.
        /// </summary>
        /// <returns>Stop 상태면 true, 아니면 false</returns>
        bool IsPaused() const;

        /// <summary>
        /// 현재 상태가 Exit 상태인지 확인합니다.
        /// </summary>
        /// <returns>Exit 상태면 true, 아니면 false</returns>
        bool IsTerminated() const;

        /// <summary>
        /// Stop 상태에서 벗어날 때까지 대기합니다.
        /// Run 또는 Exit 상태가 될 때까지 현재 스레드를 블로킹합니다.
        /// </summary>
        void WaitForNotPaused();

        /// <summary>
        /// 상태 변경을 대기합니다.
        /// timeoutMs 밀리초 동안 대기하며, 0 이상이면 타임아웃을 설정합니다.
        /// </summary>
        /// <param name="timeoutMs">대기 시간(밀리초), 0 미만이면 무한 대기</param>
        void WaitStateChange(long timeoutMs = -1);

    private:
        mutable std::mutex m_mutex;
        std::condition_variable m_condition;
        SequenceState m_currentState;

        /// <summary>
        /// 상태 전환 로직
        /// </summary>
        void ChangeState(SequenceState newState);
    };

    using SequenceStateMachinePtr = std::shared_ptr<SequenceStateMachine>;
}
