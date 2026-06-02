#include "stdafx.h"
#include "Context.h"
#include "Utils.h"

namespace EC
{
    Context::Context()
        : m_runState(RunState::Resume)
    {
    }

    Context::~Context() = default;

    void Context::SetResume()
    {
        m_runState = RunState::Resume;
    }

    void Context::SetPause()
    {
        m_runState = RunState::Pause;
    }

    void Context::SetStop()
    {
        m_runState = RunState::Stop;
    }

    bool Context::isResume() const
    {
        return m_runState == RunState::Resume;
    }

    bool Context::isPause() const
    {
        return m_runState == RunState::Pause;
    }

    bool Context::isStop() const
    {
        return m_runState == RunState::Stop;
    }

    bool Context::GetStopRequested() const
    {
        return m_runState == RunState::Stop;
    }

    void Context::SetLastError(const std::string& error)
    {
        LockGuardType guard(m_mutex);
        m_lastError = error;
    }

    std::string Context::GetLastError() const
    {
        LockGuardType guard(m_mutex);
        return m_lastError;
    }

    void Context::SetParams(const StringMap& params)
    {
        LockGuardType guard(m_mutex);
        m_params = params;
    }

    std::string Context::GetParam(const std::string& key) const
    {
        LockGuardType guard(m_mutex);
        auto it = m_params.find(key);
        if (it != m_params.end())
            return it->second;
        return std::string();
    }
}


