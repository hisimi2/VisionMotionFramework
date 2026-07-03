#include "stdafx.h"
#include "Context.h"

// boost 헤더 및 의존성 삭제
#include <string>
#include <algorithm>
#include <cctype>
#include <memory>
#include <thread>
#include <mutex>

namespace VMF
{
    static std::string ToLowerCopy(const std::string& s)
    {
        std::string out = s;
        std::transform(out.begin(), out.end(), out.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return out;
    }

    Context::Context()
        : m_processor()
        , m_repo()
        , m_isStopRequested(false)
    {
    }

    // 소멸자는 헤더 파일에 명시되어 있다면 cpp에 포함하지만 본 파일 구현에서는 간결한 형태로 둡니다
    Context::~Context() = default;

    void Context::SetVisionProcessor(VisionProcessorPtr vp)
    {
        LockGuardType guard(m_mutex);
        m_processor = vp;
    }

    VisionProcessorPtr Context::GetVisionProcessorInterface() const
    {
        LockGuardType guard(m_mutex);
        return m_processor;
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

    bool Context::ExecuteVisionCommand(VisionCommand cmd)
    {
        LockGuardType guard(m_mutex);

        if (!m_processor)
            return false;

        // visionParams는 더 이상 전역으로 관리하지 않음.
        // ExecuteVisionCommand는 프로세서를 통해 명령만 실행하고,
        // 파라미터는 각 Task가 미리 설정한 VisionProcessor를 통해 전달됩니다.
        bool ret = false;

		switch (cmd)
		{
		case Measure:       ret = m_processor->RequestMeasureAsync(StringMap());     break;
		case SetCok:        ret = m_processor->RequestSetCokAsync(StringMap());      break;
		case InspReady:     ret = m_processor->RequestInspReadyAsync(StringMap());   break;
		case DeviceCheck:   ret = m_processor->RequestDeviceCheckAsync(StringMap()); break;
		case Light:         ret = m_processor->RequestLightAsync(StringMap());       break;
		}
        return ret;
    }

    void Context::SetDataRepository(DataRepositoryPtr repo)
    {
        LockGuardType guard(m_mutex);
        m_repo = repo;
    }

DataRepositoryPtr Context::GetRepository() const
    {
        LockGuardType guard(m_mutex);
        return m_repo;
    }
}
