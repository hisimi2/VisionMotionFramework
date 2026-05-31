#pragma once

#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

class COPSwitch;
class Load1Parts;
class Load2Parts;

namespace EC
{
    class Context;
    class TaskBase;
}

namespace OperationThread
{
    class Load1TaskPicking;
    class ThreadLoad2;

    class ThreadsManager
    {
    public:
        explicit ThreadsManager(COPSwitch* startSwitch);
        ~ThreadsManager();

        /// <summary>
        /// 시퀀스 모두 시작
        /// </summary>
        void Start();

        /// <summary>
        /// 시퀀스 모두 종료
        /// </summary>
        void Stop();

        /// <summary>
        /// 시퀀스 모두 완료 여부
        /// </summary>
        bool IsComplete();

    private:
        struct TaskRunner
        {
            std::shared_ptr<EC::TaskBase>       task;
            std::shared_ptr<EC::Context>        context;
            std::thread                         thread;
            std::atomic<bool>                   completed;
            std::atomic<bool>                   stopRequested;

            TaskRunner()
                : completed(false)
                , stopRequested(false)
            {
            }
        };

        /// <summary>
        /// startSwitch 모니터링 및 제어 스레드
        /// </summary>
        void SwitchMonitoringThread();

        /// <summary>
        /// 스위치 상태 모니터링 루프 (실행 중)
        /// </summary>
        void MonitorLoop();

        int AddTaskRunner(std::shared_ptr<EC::TaskBase> task, std::shared_ptr<EC::Context> ctx);

        COPSwitch* m_startSwitch;

        std::vector<std::unique_ptr<TaskRunner>> m_runners;
        std::thread m_monitoringThread;

        std::atomic<bool> m_running;
        std::atomic<bool> m_stopRequested;

        mutable std::mutex m_mutex;
    };

    using ThreadsManagerPtr = std::shared_ptr<ThreadsManager>;
}
