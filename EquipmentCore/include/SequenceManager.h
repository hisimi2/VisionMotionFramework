#pragma once

#include "SequenceStateMachine.h"
#include "SequenceExecutor.h"
#include "ISequenceExecutable.h"
#include <memory>

namespace EquipmentCore
{
    /// <summary>
    /// 시퀀스 실행을 관리하는 고수준 매니저
    /// 상태머신, 실행기, 시퀀스를 통합적으로 관리합니다.
    /// </summary>
    class EQUIPMENT_API SequenceManager
    {
    public:
        SequenceManager();
        ~SequenceManager();

        /// <summary>
        /// 시퀀스 실행을 시작합니다.
        /// </summary>
        /// <param name="executable">실행할 시퀀스 로직</param>
        void Start(SequenceExecutablePtr executable);

        /// <summary>
        /// 시퀀스 실행을 일시 중지합니다.
        /// </summary>
        void Pause();

        /// <summary>
        /// 시퀀스 실행을 재개합니다.
        /// </summary>
        void Resume();

        /// <summary>
        /// 시퀀스 실행을 완전히 종료합니다.
        /// </summary>
        void Terminate();

        /// <summary>
        /// 현재 상태를 반환합니다.
        /// </summary>
        SequenceStateMachine::SequenceState GetState() const;

        /// <summary>
        /// 상태 문자열을 반환합니다.
        /// </summary>
        std::string GetStateString() const;

        /// <summary>
        /// 시퀀스 실행 완료 여부를 확인합니다.
        /// </summary>
        bool IsComplete() const;

        /// <summary>
        /// 마지막 오류 메시지를 반환합니다.
        /// </summary>
        std::string GetLastError() const;

        /// <summary>
        /// 실행 완료까지 대기합니다.
        /// </summary>
        void WaitForCompletion();

    private:
        SequenceStateMachinePtr m_stateMachine;
        SequenceExecutorPtr m_executor;
    };

    using SequenceManagerPtr = std::shared_ptr<SequenceManager>;
}
