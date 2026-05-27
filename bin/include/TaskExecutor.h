#pragma once
#include <cstddef>
#include <functional>
#include <memory>

#include "IScheduler.h"

namespace VC
{
    using WorkTask = std::function<void()>;

    /**
     * @brief 작업 큐 기반 비동기 실행기.
     *
     * 현재 `IScheduler` 인터페이스를 구현하지만,
     * 역할상 시간 기반 스케줄러보다는 작업 실행기(Executor)에 가깝습니다.
     *
     * - `delayMs == 0` : 즉시 실행
     * - `delayMs > 0`  : 지정 시간 이후 실행
     */
    class TaskExecutor : public IScheduler
    {
    public:
        explicit TaskExecutor(size_t threadCount = 1);
        ~TaskExecutor() override;

        void Schedule(const SchedulerTask& task, int delayMs = 0) override;
        void Start() override;
        void Stop() override;

        void Enqueue(WorkTask t);

    private:
        struct Impl;
        std::unique_ptr<Impl> m_pImpl;

        TaskExecutor(const TaskExecutor&) = delete;
        TaskExecutor& operator=(const TaskExecutor&) = delete;
    };
}

