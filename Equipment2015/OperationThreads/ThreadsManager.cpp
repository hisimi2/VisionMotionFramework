#include "stdafx.h"
#include "ThreadsManager.h"

#include <iostream>
#include <exception>

namespace OperationThread
{
    ThreadsManager::ThreadsManager()
        : m_initialized(false)
    {
    }

    ThreadsManager::~ThreadsManager()
    {
        StopAll();
    }

    void ThreadsManager::Initialize()
    {
        if (m_initialized)
        {
            m_mgr.Clear();
        }

        try
        {
            // === Load1 등록 ===
            m_mgr.RegisterActivity<CLoad1ActivityBuilder>("Load1");
            std::cout << "[ThreadsManager] Registered Load1 activity" << std::endl;

            // === Load2 등록 ===
            m_mgr.RegisterActivity<CLoad2ActivityBuilder>("Load2");
            std::cout << "[ThreadsManager] Registered Load2 activity" << std::endl;

            // === 기본 Observer 등록 (로그 출력) ===
            m_mgr.AddObserver([](const std::string& name, int requestId, const std::vector<std::string>& results) {
                std::cout << "[ThreadsManager] Activity [" << name 
                          << "] completed (requestId=" << requestId 
                          << ", results=" << results.size() << ")" << std::endl;
            });

            m_initialized = true;
            std::cout << "[ThreadsManager] Initialization complete (" 
                      << m_mgr.GetActivityCount() << " activities registered)" << std::endl;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "[ThreadsManager] Initialization failed: " << ex.what() << std::endl;
            m_initialized = false;
        }
        catch (...)
        {
            std::cerr << "[ThreadsManager] Initialization failed: unknown exception" << std::endl;
            m_initialized = false;
        }
    }

    void ThreadsManager::RunAll()
    {
        if (!m_initialized)
        {
            std::cerr << "[ThreadsManager] Not initialized. Call Initialize() first." << std::endl;
            return;
        }

        std::cout << "[ThreadsManager] Running all activities..." << std::endl;
        m_mgr.RunAll();
        std::cout << "[ThreadsManager] All activities running." << std::endl;
    }

    void ThreadsManager::PauseAll()
    {
        std::cout << "[ThreadsManager] Pausing all activities..." << std::endl;
        m_mgr.PauseAll();
        std::cout << "[ThreadsManager] All activities paused." << std::endl;
    }

    void ThreadsManager::StopAll()
    {
        std::cout << "[ThreadsManager] Stopping all activities..." << std::endl;
        m_mgr.StopAll();
        std::cout << "[ThreadsManager] All activities stopped." << std::endl;
    }

    

    bool ThreadsManager::IsRunning(const std::string& name) const
    {
        return m_mgr.IsRunning(name);
    }

    std::vector<std::string> ThreadsManager::GetActivityNames() const
    {
        return m_mgr.GetActivityNames();
    }

    ActivityId ThreadsManager::AddObserver(ActivityObserver observer)
    {
        return m_mgr.AddObserver(std::move(observer));
    }

    bool ThreadsManager::RemoveObserver(ActivityId id)
    {
        return m_mgr.RemoveObserver(id);
    }

    ActivityManager& ThreadsManager::GetManager()
    {
        return m_mgr;
    }
}
