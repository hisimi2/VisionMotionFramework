#pragma once

#include <vector>
#include <memory>
#include <thread>
#include <mutex>

#include "ISequenceExecutable.h"
#include "SequenceManager.h"

class COPSwitch;
class Load1Parts;
class Load2Parts;

namespace OperationThread
{
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
        /// <summary>
        /// startSwitch 모니터링 및 제어 스레드
        /// </summary>
        void SwitchMonitoringThread();

        /// <summary>
        /// 스위치 상태 모니터링 루프 (실행 중)
        /// </summary>
        void MonitorLoop();

        COPSwitch* m_startSwitch;

        std::vector<EC::SequenceManagerPtr> m_Managers;
        std::thread m_monitoringThread;

        bool m_running;
        bool m_stopRequested;

        mutable std::mutex m_mutex;

        int AddThread(EC::SequenceExecutablePtr sequence);
    };

    using ThreadsManagerPtr = std::shared_ptr<ThreadsManager>;
}
