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
    /// <summary>
    /// Load1과 Load2 Pick & Place 작업을 멀티스레드로 관리하는 매니저 클래스

    /// </summary>
    class AutomatedThreadsManager
    {
    public:
        explicit AutomatedThreadsManager(COPSwitch* startSwitch);
        ~AutomatedThreadsManager();

        /// <summary>
        /// Load1과 Load2 Pick & Place 시작 (멀티스레드)
        /// </summary>
        /// <param name="load1Parts">Load1Parts 객체</param>
        /// <param name="load2Parts">Load2Parts 객체</param>
        /// <param name="repeatCount">반복 횟수 (0 = 무한 반복)</param>
        void Start();

        /// <summary>
        /// 두 시퀀스 모두 종료
        /// </summary>
        void Stop();


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

        int AddSequence(EC::SequenceExecutablePtr sequence);
    };

    using AutomatedThreadsManagerPtr = std::shared_ptr<AutomatedThreadsManager>;
}
