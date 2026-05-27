#pragma once

#ifndef EQUIPMENTCORE_H
#define EQUIPMENTCORE_H

// 상태머신 및 실행 엔진
#include "SequenceStateMachine.h"
#include "SequenceExecutor.h"
#include "SequenceManager.h"
#include "ISequenceExecutable.h"

namespace EC
{
    // 편의용 타입 정의
    using StateMachinePtr = SequenceStateMachinePtr;
    using ExecutorPtr = SequenceExecutorPtr;
    using ManagerPtr = SequenceManagerPtr;
    using ExecutablePtr = SequenceExecutablePtr;
    using SequenceState = SequenceStateMachine::SequenceState;
}

#endif // EQUIPMENTCORE_H
