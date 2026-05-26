#pragma once

#include "SequenceStateMachine.h"
#include "ISequenceExecutable.h"
#include <thread>
#include <memory>



namespace EquipmentCore
{
    /// <summary>
    /// 상태머신과 함께 시퀀스를 스레드에서 실행하는 실행기
    /// </summary>
    class EQUIPMENT_API SequenceExecutor
    {
    public:
        explicit SequenceExecutor(SequenceStateMachinePtr stateMachine);
        ~SequenceExecutor();

        /// <summary>
        /// 시퀀스 실행을 시작합니다(별도 스레드에서 비동기 실행).
        /// </summary>
        /// <param name="executable">실행할 시퀀스 로직</param>
        void ExecuteAsync(SequenceExecutablePtr executable);

        /// <summary>
        /// 현재 실행 중인 스레드가 완료될 때까지 대기합니다.
        /// </summary>
        void WaitForCompletion();

        /// <summary>
        /// 현재 실행 중인 시퀀스의 완료 여부를 확인합니다.
        /// </summary>
        /// <returns>완료되었으면 true, 진행 중이면 false</returns>
        bool IsExecutionComplete() const;

        /// <summary>
        /// 마지막 오류 메시지를 반환합니다.
        /// </summary>
        /// <returns>오류 메시지</returns>
        std::string GetLastError() const;

    private:
        SequenceStateMachinePtr m_stateMachine;
        std::thread m_executionThread;
        bool m_isComplete;
        std::string m_lastError;
        mutable std::mutex m_mutex;

        /// <summary>
        /// 스레드 실행 함수
        /// </summary>
        void ExecuteSequenceThread(SequenceExecutablePtr executable);
    };

    using SequenceExecutorPtr = std::shared_ptr<SequenceExecutor>;
}
