#pragma once

#include <memory>
#include <thread>
#include <mutex>

class COPSwitch;
class Load1Parts;
class Load2Parts;

namespace OperationThread
{
    class Load1PickPlaceManager;
    class Load2PickPlaceManager;

    /// <summary>
    /// Load1과 Load2의 Pick & Place 시퀀스를 하나의 스위치로 제어하는 통합 컨트롤러
    /// startSwitch 상태에 따라 두 시퀀스가 동시에 Run/Pause 상태를 유지한다.
    /// </summary>
    class DualLoadPickPlaceController
    {
    public:
        explicit DualLoadPickPlaceController(COPSwitch* startSwitch);
        ~DualLoadPickPlaceController();

        /// <summary>
        /// Load1과 Load2 Pick & Place 시작 (멀티스레드)
        /// </summary>
        /// <param name="load1Parts">Load1Parts 객체</param>
        /// <param name="load2Parts">Load2Parts 객체</param>
        /// <param name="repeatCount">반복 횟수 (0 = 무한 반복)</param>
        void Start(Load1Parts* load1Parts, Load2Parts* load2Parts, int repeatCount = 1);

        /// <summary>
        /// 두 시퀀스 모두 종료
        /// </summary>
        void Stop();

        /// <summary>
        /// Load1 완료 여부
        /// </summary>
        bool IsLoad1Complete() const;

        /// <summary>
        /// Load2 완료 여부
        /// </summary>
        bool IsLoad2Complete() const;

        /// <summary>
        /// 둘 다 완료 여부
        /// </summary>
        bool IsBothComplete() const;

        /// <summary>
        /// Load1 상태 조회
        /// </summary>
        std::string GetLoad1StateString() const;

        /// <summary>
        /// Load2 상태 조회
        /// </summary>
        std::string GetLoad2StateString() const;

        /// <summary>
        /// Load1 현재 반복 횟수
        /// </summary>
        int GetLoad1CurrentIteration() const;

        /// <summary>
        /// Load2 현재 반복 횟수
        /// </summary>
        int GetLoad2CurrentIteration() const;

        /// <summary>
        /// Load1 성공 작업 수
        /// </summary>
        int GetLoad1SuccessCount() const;

        /// <summary>
        /// Load2 성공 작업 수
        /// </summary>
        int GetLoad2SuccessCount() const;

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
        std::shared_ptr<Load1PickPlaceManager> m_load1Manager;
        std::shared_ptr<Load2PickPlaceManager> m_load2Manager;

        std::thread m_monitoringThread;
        bool m_running;
        bool m_stopRequested;

        mutable std::mutex m_mutex;
    };

    using DualLoadPickPlaceControllerPtr = std::shared_ptr<DualLoadPickPlaceController>;
}
