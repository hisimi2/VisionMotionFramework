#include "stdafx.h"
#include "DualLoadPickPlaceController.h"
#include "Load1PickPlaceManager.h"
#include "Load2PickPlaceManager.h"
#include "Actuators/COPSwitch.h"
#include <iostream>
#include <mutex>
#include <chrono>

namespace OperationThread
{
    DualLoadPickPlaceController::DualLoadPickPlaceController(COPSwitch* startSwitch)
        : m_startSwitch(startSwitch)
        , m_running(false)
        , m_stopRequested(false)
    {
        m_load1Manager = std::make_shared<Load1PickPlaceManager>();
        m_load2Manager = std::make_shared<Load2PickPlaceManager>();
    }

    DualLoadPickPlaceController::~DualLoadPickPlaceController()
    {
        Stop();
    }

    void DualLoadPickPlaceController::Start(Load1Parts* load1Parts, Load2Parts* load2Parts, int repeatCount)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_running)
        {
            std::cerr << "Already running. Stop before starting again." << std::endl;
                return;
        }

        m_running = true;
        m_stopRequested = false;

        // Load1과 Load2 시퀀스 시작
        m_load1Manager->Start(load1Parts, repeatCount);
        m_load2Manager->Start(load2Parts, repeatCount);

        std::cout << "[DualLoadPickPlaceController] Load1 and Load2 sequences started" << std::endl;

        // 스위치 모니터링 스레드 시작
        m_monitoringThread = std::thread(&DualLoadPickPlaceController::SwitchMonitoringThread, this);
    }

    void DualLoadPickPlaceController::Stop()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stopRequested = true;
            m_running = false;
        }

        // 모니터링 스레드 종료 대기
        if (m_monitoringThread.joinable())
        {
            m_monitoringThread.join();
        }

        // 두 매니저 종료
        if (m_load1Manager)
        {
            m_load1Manager->Terminate();
        }

        if (m_load2Manager)
        {
            m_load2Manager->Terminate();
        }

        std::cout << "[DualLoadPickPlaceController] Stopped" << std::endl;
    }

    bool DualLoadPickPlaceController::IsLoad1Complete() const
    {
        return m_load1Manager ? m_load1Manager->IsComplete() : true;
    }

    bool DualLoadPickPlaceController::IsLoad2Complete() const
    {
        return m_load2Manager ? m_load2Manager->IsComplete() : true;
    }

    bool DualLoadPickPlaceController::IsBothComplete() const
    {
        return IsLoad1Complete() && IsLoad2Complete();
    }

    std::string DualLoadPickPlaceController::GetLoad1StateString() const
    {
        return m_load1Manager ? m_load1Manager->GetStateString() : "Unknown";
    }

    std::string DualLoadPickPlaceController::GetLoad2StateString() const
    {
        return m_load2Manager ? m_load2Manager->GetStateString() : "Unknown";
    }

    int DualLoadPickPlaceController::GetLoad1CurrentIteration() const
    {
        return m_load1Manager ? m_load1Manager->GetCurrentIteration() : 0;
    }

    int DualLoadPickPlaceController::GetLoad2CurrentIteration() const
    {
        return m_load2Manager ? m_load2Manager->GetCurrentIteration() : 0;
    }

    int DualLoadPickPlaceController::GetLoad1SuccessCount() const
    {
        return m_load1Manager ? m_load1Manager->GetSuccessCount() : 0;
    }

    int DualLoadPickPlaceController::GetLoad2SuccessCount() const
    {
        return m_load2Manager ? m_load2Manager->GetSuccessCount() : 0;
    }

    void DualLoadPickPlaceController::SwitchMonitoringThread()
    {
        std::cout << "[DualLoadPickPlaceController] Switch monitoring thread started" << std::endl;

        try
        {
            while (!m_stopRequested)
            {
                    MonitorLoop();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        catch (const std::exception& ex)
        {
            std::cerr << "[DualLoadPickPlaceController] Error in monitoring thread: " << ex.what() << std::endl;
        }

        std::cout << "[DualLoadPickPlaceController] Switch monitoring thread ended" << std::endl;
    }

    void DualLoadPickPlaceController::MonitorLoop()
    {
        if (!m_startSwitch || !m_load1Manager || !m_load2Manager)
        {
            return;
        }

        // startSwitch 상태 확인
        bool switchStatus = m_startSwitch->getStatus();
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if (!m_running)
            {
                return;
            }

            // 스위치가 ON일 때: Resume
            if (switchStatus)
            {
                // 두 시퀀스가 Pause 상태이면 Resume
                std::string load1State = m_load1Manager->GetStateString();
                std::string load2State = m_load2Manager->GetStateString();

                if (load1State == "Stop")
                {
                    m_load1Manager->Resume();
                    std::cout << "[Load1] Resumed by switch" << std::endl;
                }

                if (load2State == "Stop")
                {
                    m_load2Manager->Resume();
                    std::cout << "[Load2] Resumed by switch" << std::endl;
                }
            }
            // 스위치가 OFF일 때: Pause
            else
            {
                std::string load1State = m_load1Manager->GetStateString();
                std::string load2State = m_load2Manager->GetStateString();

                if (load1State == "Run")
                {
                    m_load1Manager->Pause();
                    std::cout << "[Load1] Paused by switch" << std::endl;
                }

                if (load2State == "Run")
                {
                    m_load2Manager->Pause();
                    std::cout << "[Load2] Paused by switch" << std::endl;
                }
            }

            // 둘 다 완료되면 루프 종료
            if (IsBothComplete())
            {
                m_running = false;
                std::cout << "[DualLoadPickPlaceController] Both sequences completed" << std::endl;
            }
        }
    }
}
