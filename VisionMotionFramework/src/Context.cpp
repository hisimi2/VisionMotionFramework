#include "stdafx.h"
#include "Context.h"

#include <string>
#include <memory>
#include <thread>
#include <mutex>

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

        StringMap params = m_params.visionParams;

        return m_processor->RequestAsync(cmd, params);
    }

    void Context::SetVisionParams(const VisionParams& params)
    {
        LockGuardType guard(m_mutex);
        m_params = params;
    }

    std::string Context::GetSeqParam(const std::string& key) const
    {
        LockGuardType guard(m_mutex);
        auto it = m_params.seqParams.find(key);
        if (it != m_params.seqParams.end())
            return it->second;
        return std::string();
    }

    std::string Context::GetVisionParam(const std::string& key) const
    {
        LockGuardType guard(m_mutex);
        auto it = m_params.visionParams.find(key);
        if (it != m_params.visionParams.end())
            return it->second;
        return std::string();
    }

    void Context::SetSeqParam(const std::string& key, int value)
    {
        SetSeqParamAs<int>(key, value);
    }

    std::vector<VisionPosition> Context::GetVisionPositions() const
    {
        LockGuardType guard(m_mutex);
        return m_params.visionPositions;
    }

    bool Context::PopVisionPosition(VisionPosition& outPos)
    {
        LockGuardType guard(m_mutex);
        if (m_params.visionPositions.empty())
            return false;

        outPos = m_params.visionPositions.front();
        m_params.visionPositions.erase(m_params.visionPositions.begin());
        return true;
    }

    bool Context::PeekVisionPosition(VisionPosition& outPos)
    {
        LockGuardType guard(m_mutex);

        if (m_params.visionPositions.empty())
            return false;

        outPos = m_params.visionPositions.back();
        return true;
    }

    void Context::AddVisionPosition(const VisionPosition& pos)
    {
        LockGuardType guard(m_mutex);
        m_params.visionPositions.push_back(pos);
    }

    bool Context::IsVisionPositionEmpty() const
    {
        LockGuardType guard(m_mutex);
        return m_params.visionPositions.empty();
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
