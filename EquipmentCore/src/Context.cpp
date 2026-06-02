#include "stdafx.h"
#include "Context.h"
#include "Utils.h"

namespace EC
{
    Context::Context() : m_isRequested(Pause)
    {
    }
    
    Context::~Context() = default;

    void Context::SetResume()
    {
        m_isRequested = Resume;
    }
    void Context::SetPause()
    {
        m_isRequested = Pause;
    }
    void Context::SetStop()
    {
        m_isRequested = Stop;
    }

    bool Context::isResume() const
    {
        return m_isRequested == Resume;

    }
    bool Context::isPause() const
    {
        return m_isRequested == Pause;
    }
    bool Context::isStop() const
    {
        return m_isRequested == Stop;
    }

    void Context::SetLastError(const std::string& error)
    {
        LockGuardType guard(m_mutex);
        m_lastError = error;
    }

    const std::string& Context::GetLastError() const
    {
        // m_lastError 반환 시 동기화 여부는 설계 정책에 따르나, 기본적으로 std::string 참조형이므로 외부에선 주의가 필요합니다.
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


