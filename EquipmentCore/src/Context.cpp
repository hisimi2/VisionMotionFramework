#include "stdafx.h"
#include "Context.h"

#include "Utils.h"

namespace EC
{
    Context::Context() : m_isStopRequested(false)
    {
    }

    // 소멸자는 헤더 파일에 명시되어 있다면 cpp에 포함하지만 본 파일 구현에서는 간결한 형태로 둡니다
    Context::~Context() = default;

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

    void Context::SetStopRequested(bool stop)
    {
        LockGuardType guard(m_mutex);
        m_isStopRequested = stop;
    }

    bool Context::GetStopRequested() const
    {
        LockGuardType guard(m_mutex);
        return m_isStopRequested;
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


