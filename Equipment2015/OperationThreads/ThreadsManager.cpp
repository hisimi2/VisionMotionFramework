#include "stdafx.h"
#include "ThreadsManager.h"
#include "Load1ActivityBuilder.h"
#include "Load2ActivityBuilder.h"

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
            OutputDebugStringA("[ThreadsManager] Registered Load1 activity\n");

            // === Load2 등록 ===
            m_mgr.RegisterActivity<CLoad2ActivityBuilder>("Load2");
            OutputDebugStringA("[ThreadsManager] Registered Load2 activity\n");

            // === 기본 Observer 등록 (로그 출력) ===
            m_mgr.AddObserver([](const std::string& name, int requestId, const std::vector<std::string>& results) {
                std::string msg = "[ThreadsManager] Activity [" + name + "] completed (requestId=" + std::to_string(requestId) + ", results=" + std::to_string(results.size()) + ")\n";
                OutputDebugStringA(msg.c_str());
            });

            m_initialized = true;
            std::string msg = "[ThreadsManager] Initialization complete (" + std::to_string(m_mgr.GetActivityCount()) + " activities registered)\n";
            OutputDebugStringA(msg.c_str());
        }
        catch (const std::exception& ex)
        {
            std::string msg = std::string("[ThreadsManager] Initialization failed: ") + ex.what() + "\n";
            OutputDebugStringA(msg.c_str());
            m_initialized = false;
        }
        catch (...)
        {
            OutputDebugStringA("[ThreadsManager] Initialization failed: unknown exception\n");
            m_initialized = false;
        }
    }

    void ThreadsManager::RunAll()
    {
        if (!m_initialized)
        {
            OutputDebugStringA("[ThreadsManager] Not initialized. Call Initialize() first.\n");
            return;
        }

        OutputDebugStringA("[ThreadsManager] Running all activities...\n");
        m_mgr.RunAll();
        OutputDebugStringA("[ThreadsManager] All activities running.\n");
    }

    void ThreadsManager::PauseAll()
    {
        OutputDebugStringA("[ThreadsManager] Pausing all activities...\n");
        m_mgr.PauseAll();
        OutputDebugStringA("[ThreadsManager] All activities paused.\n");
    }

    void ThreadsManager::StopAll()
    {
        OutputDebugStringA("[ThreadsManager] Stopping all activities...\n");
        m_mgr.StopAll();
        OutputDebugStringA("[ThreadsManager] All activities stopped.\n");
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
