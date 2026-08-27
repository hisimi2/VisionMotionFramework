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

    /// <summary>
    /// Task 실행 인터페이스
    /// 각 Task는 Execute()가 반복 호출되며 상태머신을 통해 단계별 작업을 수행합니다.
    /// </summary>
    class ITask
    {
    public:
        virtual ~ITask() = default;

        /// <summary>
        /// Task 실행 (Activity에 의해 반복 호출됨)
        /// </summary>
        virtual TaskResult Execute(Context& ctx) = 0;

        /// <summary>
        /// Task 이름 반환
        /// </summary>
        virtual std::string GetName() const = 0;
    };

    using TaskPtr = std::shared_ptr<ITask>;
}
