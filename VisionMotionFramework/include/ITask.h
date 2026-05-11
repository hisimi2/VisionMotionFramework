#pragma once
#include "VMF_API.h"

#include <string>
#include <memory> // std::shared_ptr 사용을 위해 포함
// VAT_Context 객체를 참조로 사용하더라도, 인터페이스 사용자가 Context 정의를 필요로 할 가능성이 높으므로 포함
#include "VAT_Context.h" 

namespace VMF
{
    class IVatActuator;
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
        virtual TaskResult Execute(VAT_Context& ctx, IVatActuator* actuator) = 0;
        virtual TaskResult SetErrorAndReturn(VAT_Context& ctx, const std::string& msg) = 0;

        // 중단 시 필요한 정리 작업(하드웨어 안전 복귀 등)
        // 기본 구현은 아무것도 하지 않음 (void cast로 경고 방지)
        virtual void Abort() = 0;
        virtual void EnterState(int newState) = 0;
        virtual std::string GetName() const = 0;
    };

    using TaskStepPtr = std::shared_ptr<ITask>;
} // namespace VMF
