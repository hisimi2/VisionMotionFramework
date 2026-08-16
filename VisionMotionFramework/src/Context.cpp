#include "stdafx.h"
#include "Context.h"
#include "IParamProvider.h"

#include <string>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace VMF
{
    Context::Context()
        : m_processor()
        , m_repo()
        , m_isStopRequested(false)
    {
    }

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

    // ── Task 파라미터 관리 구현 (IParamProvider 인터페이스 구현) ──

    // 하위 호환성: 기본 파라미터 설정
    void Context::SetTaskParams(const TaskParams& params)
    {
        LockGuardType guard(m_mutex);
        m_defaultTaskParams = params;
        // 빈 Task 이름으로 기본 파라미터 저장
        m_taskParamsMap[""] = params;
    }

    // Task 이름 기반 파라미터 설정
    void Context::SetTaskParams(const std::string& taskName, const TaskParams& params)
    {
        LockGuardType guard(m_mutex);
        m_taskParamsMap[taskName] = params;
    }

    // 하위 호환성: 기본 파라미터 조회
    TaskParams Context::GetTaskParams() const
    {
        LockGuardType guard(m_mutex);
        return m_defaultTaskParams;
    }

    // Task 이름 기반 파라미터 조회
    TaskParams Context::GetTaskParams(const std::string& taskName) const
    {
        LockGuardType guard(m_mutex);
        auto it = m_taskParamsMap.find(taskName);
        if (it != m_taskParamsMap.end())
            return it->second;
        // Task 이름이 없는 경우 기본 파라미터 반환
        return m_defaultTaskParams;
    }

    std::string Context::GetExecutionParam(const std::string& key) const
    {
        LockGuardType guard(m_mutex);
        auto it = m_defaultTaskParams.executionParams.find(key);
        if (it != m_defaultTaskParams.executionParams.end())
            return it->second;
        return "";
    }

    void Context::SetExecutionParam(const std::string& key, const std::string& value)
    {
        LockGuardType guard(m_mutex);
        m_defaultTaskParams.executionParams[key] = value;
    }

    void Context::SetExecutionParam(const std::string& key, int value)
    {
        LockGuardType guard(m_mutex);
        m_defaultTaskParams.executionParams[key] = std::to_string(value);
    }

    void Context::SetExecutionParam(const std::string& key, double value)
    {
        LockGuardType guard(m_mutex);
        m_defaultTaskParams.executionParams[key] = std::to_string(value);
    }

    std::vector<VisionPosition> Context::GetVisionPositions() const
    {
        LockGuardType guard(m_mutex);
        return m_defaultTaskParams.visionPositions;
    }

    bool Context::PeekVisionPosition(VisionPosition& outPos) const
    {
        LockGuardType guard(m_mutex);
        if (m_defaultTaskParams.visionPositions.empty())
            return false;
        outPos = m_defaultTaskParams.visionPositions.back();
        return true;
    }

}
