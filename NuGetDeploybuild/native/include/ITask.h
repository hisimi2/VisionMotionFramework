#pragma once
#include "VMF_API.h"

#include <string>
#include <memory> 
#include "Context.h" 

namespace VMF
{
    class IActuator;
    class IDataRepository;

    enum TaskResult
    {
        TR_KEEP,    // 현재 Task 유지(다음 주기에 다시 실행)
        TR_NEXT,    // 다음 Task으로 전환
        TR_PREV,    // 이전 Task으로 전환
        TR_DONE,    // 전체 Task 완료(명시적)
        TR_ERROR    // 에러 발생(시퀀스 중단)
    };
    

    /**
     * @class ITask
     * @brief 시퀀스를 구성하는 단위 작업(Step) 인터페이스
     */
    class VMF_API ITask
    {
    public:
        virtual ~ITask() = default;

        // 비차단 실행 함수: 호출할 때마다 짧게 실행되어야 함.
        virtual TaskResult Execute(Context& ctx, IActuator* actuator) = 0;
        virtual TaskResult SetErrorAndReturn(Context& ctx, const std::string& msg) = 0;

        // 중단 시 필요한 정리 작업(하드웨어 안전 복귀 등)
        virtual void Abort() = 0;
        virtual void EnterState(int newState) = 0;
        virtual std::string GetName() const = 0;
    };

    using TaskPtr = std::shared_ptr<ITask>;
} // namespace VMF
