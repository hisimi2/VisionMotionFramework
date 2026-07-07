#pragma once

#include "EC_API.h"
#include "ActivityManager.h"
#include "ActivityBuilderBase.h"

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <atomic>
#include <functional>
#include <type_traits>

namespace EC
{
    // ThreadsManager state enum
    enum class ThreadsState
    {
        Idle,
        Running,
        Paused,
        Stopped
    };

    // BuilderFactory - factory function that creates ActivityBuilder (no Parts dependency)
    using BuilderFactory = std::function<ActivityBuilderPtr()>;

    class EC_API ThreadsManager
    {
    public:
        ThreadsManager();
        ~ThreadsManager();

        // === Builder registration (3 ways) ===

        // Way 1: Template registration - uses default constructor of T
        template<typename T>
        typename std::enable_if<std::is_base_of<ActivityBuilderBase, T>::value, void>::type
        RegisterBuilder(const std::string& name)
        {
            if (m_initialized)
            {
                OutputDebugStringA("[ThreadsManager] Already initialized. Cannot register builder.\n");
                return;
            }
            m_factories[name] = []() -> ActivityBuilderPtr {
                return std::make_shared<T>();
            };
            std::string msg = "[ThreadsManager] Builder registered (template): " + name + "\n";
            OutputDebugStringA(msg.c_str());
        }

        // Way 2: Direct builder instance registration (each builder already has its own Parts)
        void RegisterBuilder(const std::string& name, ActivityBuilderPtr builder);

        // Way 3: Factory function registration (custom creation logic)
        void RegisterBuilder(const std::string& name, BuilderFactory factory);

        // Get number of registered builders
        size_t GetBuilderCount() const { return m_factories.size(); }

        // Get list of registered builder names
        std::vector<std::string> GetBuilderNames() const;

        // === Initialization ===

        // Initialize - registers all builder factories with ActivityManager
        void Initialize();

        // Whether initialized
        bool IsInitialized() const { return m_initialized; }

        // === Execution control ===

        // Run all activities (first run or resume from pause)
        void RunAll();

        // Pause all activities
        void PauseAll();

        // Stop all activities
        void StopAll();

        // === State query ===

        ThreadsState GetState() const { return m_state.load(); }
        bool IsRunning() const { return m_state.load() == ThreadsState::Running; }
        bool IsPaused() const { return m_state.load() == ThreadsState::Paused; }
        bool IsActivityRunning(const std::string& name) const;

        // === Observer ===

        ActivityId AddObserver(ActivityObserver observer);
        bool RemoveObserver(ActivityId id);

        // Direct access to ActivityManager (for advanced control)
        ActivityManager& GetManager();

    private:
        void SetState(ThreadsState newState);

        ActivityManager                         m_mgr;
        std::map<std::string, BuilderFactory>   m_factories;
        std::atomic<ThreadsState>               m_state{ ThreadsState::Idle };
        bool                                    m_initialized = false;
    };
}

