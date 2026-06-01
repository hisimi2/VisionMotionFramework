#include "stdafx.h"
#include "ThreadsManager.h"
#include "Actuators/COPSwitch.h"
#include <iostream>
#include <mutex>
#include <chrono>

#include "TaskBase.h"
#include "Context.h"

#include "Actuators/Load1Parts.h"

#include "Load1TaskPick.h"

namespace OperationThread
{
    ThreadsManager::ThreadsManager(COPSwitch* startSwitch)
        : m_startSwitch(startSwitch)
        , m_running(false)
        , m_stopRequested(false)
    {
    }

    ThreadsManager::~ThreadsManager()
    {
        Stop();
    }

    int ThreadsManager::AddTaskRunner(std::shared_ptr<EC::TaskBase> task, std::shared_ptr<EC::Context> ctx)
    {
        auto runner = std::make_unique<TaskRunner>();
        runner->task = task;
        runner->context = ctx;

        m_runners.push_back(std::move(runner));
        return static_cast<int>(m_runners.size() - 1);
    }

    void ThreadsManager::Start()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_running)
        {
            std::cerr << "Already running. Stop before starting again." << std::endl;
            return;
        }

        // TaskRunner 초기화
        m_runners.clear();

        // Load1 Task
        {
            auto ctx = std::make_shared<EC::Context>();
            auto task1 = std::make_shared<Load1TaskPick>((LPVOID)nullptr);
            AddTaskRunner(task1, ctx);
        }


        m_running = true;
        m_stopRequested = false;

        // 각 TaskRunner의 스레드 시작
        for (auto& runner : m_runners)
        {
            auto task = runner->task;
            auto ctx = runner->context;
            auto completed = &runner->completed;
            auto stopReq = &runner->stopRequested;

            runner->thread = std::thread(
                [task, ctx, completed, stopReq]()
                {
                    while (!stopReq->load())
                    {
                        if (ctx->GetStopRequested())
                        {
                            std::this_thread::sleep_for(std::chrono::milliseconds(10));
                            continue;
                        }

                        EC::TaskResult res = task->Execute(*ctx);
                        if (res == EC::TR_NEXT || res == EC::TR_DONE)
                        {
                            completed->store(true);
                            return;
                        }
                        else if (res == EC::TR_ERROR)
                        {
                            completed->store(true);
                            return;
                        }

                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                    completed->store(true);
                });
        }

        std::cout << "[ThreadsManager] Load1 and Load2 sequences started" << std::endl;

        // 스위치 모니터링 스레드 시작
        m_monitoringThread = std::thread(&ThreadsManager::SwitchMonitoringThread, this);
    }

    void ThreadsManager::Stop()
    {
        m_stopRequested = true;
        m_running = false;

        // 모든 실행 중인 스레드에 중단 요청
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (auto& runner : m_runners)
            {
                runner->stopRequested = true;
                if (runner->context)
                {
                    runner->context->SetStopRequested(true);
                }
            }
        }

        // 모든 스레드 join
        for (auto& runner : m_runners)
        {
            if (runner->thread.joinable())
            {
                runner->thread.join();
            }
        }

        // 모니터링 스레드 종료 대기
        if (m_monitoringThread.joinable())
        {
            m_monitoringThread.join();
        }

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_runners.clear();
        }

        std::cout << "[ThreadsManager] Stopped" << std::endl;
    }

    void ThreadsManager::SwitchMonitoringThread()
    {
        std::cout << "[ThreadsManager] Switch monitoring thread started" << std::endl;

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
            std::cerr << "[ThreadsManager] Error in monitoring thread: " << ex.what() << std::endl;
        }

        std::cout << "[ThreadsManager] Switch monitoring thread ended" << std::endl;
    }

    bool ThreadsManager::IsComplete()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& runner : m_runners)
        {
            if (!runner->completed.load())
            {
                return false;
            }
        }
        return true;
    }

    void ThreadsManager::MonitorLoop()
    {
        if (!m_startSwitch || !m_running)
        {
            return;
        }

        bool switchStatus = m_startSwitch->getStatus();

        std::lock_guard<std::mutex> lock(m_mutex);

        // 스위치가 OFF일 때: 일시 중지 (Stop 요청)
        if (!switchStatus)
        {
            for (const auto& runner : m_runners)
            {
                if (runner->context)
                {
                    runner->context->SetStopRequested(true);
                }
            }
        }
        // 스위치가 ON일 때: 재개 (Stop 요청 해제)
        else
        {
            for (const auto& runner : m_runners)
            {
                if (runner->context)
                {
                    runner->context->SetStopRequested(false);
                }
            }
        }

        // 완료되면 루프 종료
        if (IsComplete())
        {
            m_running = false;
            std::cout << "[ThreadsManager] Both sequences completed" << std::endl;
        }
    }
}
