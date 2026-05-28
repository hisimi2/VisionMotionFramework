#include "stdafx.h"
#include "AutomatedThreadsManager.h"
#include "Actuators/COPSwitch.h"
#include <iostream>
#include <mutex>
#include <chrono>

#include "Actuators/Load1Parts.h"
#include "Actuators/Load2Parts.h"

#include "Load1RobotSequence.h"
#include "Load2RobotSequence.h"

namespace OperationThread
{
    AutomatedThreadsManager::AutomatedThreadsManager(COPSwitch* startSwitch)
        : m_startSwitch(startSwitch)
        , m_running(false)
        , m_stopRequested(false)
    {
        int repeatCount = 2;
        Load1Parts parts;

        AddSequence(std::make_shared<Load1RobotSequence>((LPVOID)&parts, repeatCount));
    }

    int AutomatedThreadsManager::AddSequence(EC::SequenceExecutablePtr sequence)
    {
        m_Managers.push_back(std::make_shared<EC::SequenceManager>(sequence));

        return static_cast<int>(m_Managers.size() - 1);
    }

    AutomatedThreadsManager::~AutomatedThreadsManager()
    {
        Stop();
    }

    void AutomatedThreadsManager::Start()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_running)
        {
            std::cerr << "Already running. Stop before starting again." << std::endl;
            return;
        }

        m_running = true;
        m_stopRequested = false;

        for (const auto& manager : m_Managers)
        {
            manager->Start();
        }

        std::cout << "[AutomatedThreadsManager] Load1 and Load2 sequences started" << std::endl;

        // 스위치 모니터링 스레드 시작
        m_monitoringThread = std::thread(&AutomatedThreadsManager::SwitchMonitoringThread, this);
    }

    void AutomatedThreadsManager::Stop()
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

        for (const auto& manager : m_Managers)
        {
            manager->Terminate();
        }

        std::cout << "[AutomatedThreadsManager] Stopped" << std::endl;
    }

    void AutomatedThreadsManager::SwitchMonitoringThread()
    {
        std::cout << "[AutomatedThreadsManager] Switch monitoring thread started" << std::endl;

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
            std::cerr << "[AutomatedThreadsManager] Error in monitoring thread: " << ex.what() << std::endl;
        }

        std::cout << "[AutomatedThreadsManager] Switch monitoring thread ended" << std::endl;
    }

    bool AutomatedThreadsManager::IsComplete()
    {
        for (const auto& manager : m_Managers)
        {
            if (!manager->IsComplete())
            {
                return false;
            }
        }
        return true;
    }

    void AutomatedThreadsManager::MonitorLoop()
    {
        if (!m_startSwitch)
        {
            return;
        }

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
                for (const auto& manager : m_Managers)
                {
                    std::string state = manager->GetStateString();
                    if (state == "Stop")
                    {
                        manager->Resume();
                        std::cout << "[Manager] Resumed by switch" << std::endl;
                    }

                }
            }
            // 스위치가 OFF일 때: Pause
            else
            {
                for (const auto& manager : m_Managers)
                {
                    std::string state = manager->GetStateString();
                    if (state == "Run")
                    {
                        manager->Pause();
                        std::cout << "[Manager] Paused by switch" << std::endl;
                    }
                }
            }

            // 완료되면 루프 종료
            if (IsComplete())
            {
                m_running = false;
                std::cout << "[AutomatedThreadsManager] Both sequences completed" << std::endl;
            }
        }
    }
}
