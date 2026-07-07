#include "stdafx.h"
#include "ThreadsManager.h"
#include <exception>

namespace EC
{
    ThreadsManager::ThreadsManager()
        : m_initialized(false)
        , m_state(ThreadsState::Idle)
    {
    }

    ThreadsManager::~ThreadsManager()
    {
        StopAll();
    }

    // ============ Builder registration (non-template overloads) ============

    void ThreadsManager::RegisterBuilder(const std::string& name, ActivityBuilderPtr builder)
    {
        if (m_initialized)
        {
            OutputDebugStringA("[ThreadsManager] Already initialized. Cannot register builder.\n");
            return;
        }
        m_factories[name] = [builder]() -> ActivityBuilderPtr {
            return builder;
        };
        std::string msg = "[ThreadsManager] Builder registered (direct): " + name + "\n";
        OutputDebugStringA(msg.c_str());
    }

    void ThreadsManager::RegisterBuilder(const std::string& name, BuilderFactory factory)
    {
        if (m_initialized)
        {
            OutputDebugStringA("[ThreadsManager] Already initialized. Cannot register builder.\n");
            return;
        }
        m_factories[name] = std::move(factory);
        std::string msg = "[ThreadsManager] Builder registered (factory): " + name + "\n";
        OutputDebugStringA(msg.c_str());
    }

    // ============ Builder name list ============

    std::vector<std::string> ThreadsManager::GetBuilderNames() const
    {
        std::vector<std::string> names;
        names.reserve(m_factories.size());
        for (const auto& pair : m_factories)
        {
            names.push_back(pair.first);
        }
        return names;
    }

    // ============ State management ============

    void ThreadsManager::SetState(ThreadsState newState)
    {
        ThreadsState oldState = m_state.exchange(newState);

        std::string msg = "[ThreadsManager] State: " +
            std::to_string(static_cast<int>(oldState)) + " -> " +
            std::to_string(static_cast<int>(newState)) + "\n";
        OutputDebugStringA(msg.c_str());
    }

    // ============ Initialization ============

    void ThreadsManager::Initialize()
    {
        if (m_initialized)
        {
            StopAll();
            m_mgr.Clear();
        }

        try
        {
            if (m_factories.empty())
            {
                OutputDebugStringA("[ThreadsManager] No builders registered. Nothing to initialize.\n");
                m_initialized = true;
                SetState(ThreadsState::Idle);
                return;
            }

            for (auto it = m_factories.begin(); it != m_factories.end(); ++it)
            {
                const std::string& name = it->first;
                BuilderFactory& factory = it->second;

                auto builder = factory();
                if (!builder)
                {
                    std::string errMsg = "[ThreadsManager] Builder factory for [" + name + "] returned null. Skipping.\n";
                    OutputDebugStringA(errMsg.c_str());
                    continue;
                }

                bool registered = m_mgr.RegisterActivityWithBuilder(name, builder);
                if (registered)
                {
                    std::string msg = "[ThreadsManager] Registered activity [" + name + "]\n";
                    OutputDebugStringA(msg.c_str());
                }
                else
                {
                    std::string errMsg = "[ThreadsManager] Failed to register activity [" + name + "]\n";
                    OutputDebugStringA(errMsg.c_str());
                }
            }

            // Default observer (log output)
            m_mgr.AddObserver([](const std::string& name, int requestId, const std::vector<std::string>& results) {
                std::string msg = "[ThreadsManager] Activity [" + name + "] completed (requestId=" + std::to_string(requestId) + ", results=" + std::to_string(results.size()) + ")\n";
                OutputDebugStringA(msg.c_str());
            });

            m_initialized = true;
            SetState(ThreadsState::Idle);

            std::string msg = "[ThreadsManager] Initialization complete (" + std::to_string(m_mgr.GetActivityCount()) + " activities registered from " + std::to_string(m_factories.size()) + " builders)\n";
            OutputDebugStringA(msg.c_str());
        }
        catch (const std::exception& ex)
        {
            std::string msg = std::string("[ThreadsManager] Initialization failed: ") + ex.what() + "\n";
            OutputDebugStringA(msg.c_str());
            m_initialized = false;
            SetState(ThreadsState::Stopped);
        }
        catch (...)
        {
            OutputDebugStringA("[ThreadsManager] Initialization failed: unknown exception\n");
            m_initialized = false;
            SetState(ThreadsState::Stopped);
        }
    }

    // ============ Execution control ============

    void ThreadsManager::RunAll()
    {
        if (!m_initialized)
        {
            OutputDebugStringA("[ThreadsManager] Not initialized. Call Initialize() first.\n");
            return;
        }

        ThreadsState current = m_state.load();

        switch (current)
        {
        case ThreadsState::Running:
            OutputDebugStringA("[ThreadsManager] Already running. Ignoring RunAll().\n");
            return;

        case ThreadsState::Paused:
            OutputDebugStringA("[ThreadsManager] Resuming all activities...\n");
            m_mgr.RunAll();
            SetState(ThreadsState::Running);
            return;

        case ThreadsState::Idle:
        case ThreadsState::Stopped:
            OutputDebugStringA("[ThreadsManager] Starting all activities...\n");
            m_mgr.RunAll();
            SetState(ThreadsState::Running);
            return;
        }
    }

    void ThreadsManager::PauseAll()
    {
        if (!m_initialized)
        {
            OutputDebugStringA("[ThreadsManager] Not initialized.\n");
            return;
        }

        ThreadsState current = m_state.load();

        switch (current)
        {
        case ThreadsState::Running:
            OutputDebugStringA("[ThreadsManager] Pausing all activities...\n");
            m_mgr.PauseAll();
            SetState(ThreadsState::Paused);
            break;

        case ThreadsState::Paused:
            OutputDebugStringA("[ThreadsManager] Already paused.\n");
            break;

        case ThreadsState::Idle:
        case ThreadsState::Stopped:
            OutputDebugStringA("[ThreadsManager] Not running. Cannot pause.\n");
            break;
        }
    }

    void ThreadsManager::StopAll()
    {
        ThreadsState current = m_state.load();

        if (current == ThreadsState::Idle || current == ThreadsState::Stopped)
        {
            return;
        }

        OutputDebugStringA("[ThreadsManager] Stopping all activities...\n");
        m_mgr.StopAll();
        m_mgr.Clear();

        SetState(ThreadsState::Idle);

        OutputDebugStringA("[ThreadsManager] All activities stopped.\n");
    }

    // ============ State query ============

    bool ThreadsManager::IsActivityRunning(const std::string& name) const
    {
        return m_mgr.IsRunning(name);
    }

    // ============ Observer ============

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

