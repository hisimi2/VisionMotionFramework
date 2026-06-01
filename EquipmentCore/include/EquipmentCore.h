#pragma once

#ifndef EQUIPMENTCORE_H
#define EQUIPMENTCORE_H

// 상태머신 및 실행 엔진
#include "ActivityStateMachine.h"
#include "ActivityExecutor.h"
#include "ActivityManager.h"
#include "IActivityExecutable.h"

namespace EC
{
    // 편의용 타입 정의
    using StateMachinePtr = ActivityStateMachinePtr;
    using ExecutorPtr = ActivityExecutorPtr;
    using ManagerPtr = ActivityManagerPtr;
    using ExecutablePtr = ActivityExecutablePtr;
    using ActivityState = ActivityStateMachine::ActivityState;
}

#endif // EQUIPMENTCORE_H
