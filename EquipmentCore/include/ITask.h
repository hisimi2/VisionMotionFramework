#pragma once

#include <string>
#include <memory>

namespace EC
{
    class Context;

    enum TaskResult
    {
        TR_KEEP,    // 현재 Task 유지(다음 주기에 다시 실행)
        TR_NEXT,    // 다음 Task으로 전환
        TR_PREV,    // 이전 Task으로 전환
        TR_DONE,    // 전체 Task 완료(명시적)
        TR_ERROR    // 에러 발생(시퀀스 중단)
    };


    class ITask
    {
    public:
        virtual ~ITask() = default;

        virtual TaskResult Execute(Context& ctx) = 0;
        virtual TaskResult SetErrorAndReturn(Context& ctx, const std::string& msg) = 0;

        virtual void Abort() = 0;
        virtual void EnterState(int newState) = 0;
        virtual std::string GetName() const = 0;
    };

    using TaskPtr = std::shared_ptr<ITask>;
} 
