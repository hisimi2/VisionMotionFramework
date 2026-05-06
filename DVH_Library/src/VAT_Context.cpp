#include "stdafx.h"
#include "VAT_Context.h"
#include <boost/thread/thread.hpp> 
#include <boost/shared_ptr.hpp>
#include <string>
#include <algorithm>

namespace DVH_VAT
{
    static std::string ToLowerCopy(const std::string& s)
    {
        std::string out = s;
        std::transform(out.begin(), out.end(), out.begin(), ::tolower);
        return out;
    }

    VAT_Context::VAT_Context()
        : m_processor()
        , m_repo()
        , m_isStopRequested(false)
    {
    }

    VAT_Context::~VAT_Context() {}

    void VAT_Context::SetVisionProcessor(VisionEventHandlerPtr vp)
    {
        LockGuardType guard(m_mutex);
        m_processor = vp;
    }

    VisionEventHandlerPtr VAT_Context::GetVisionProcessorInterface() const
    {
        LockGuardType guard(m_mutex);
        return m_processor;
    }

    void VAT_Context::SetLastError(const std::string& error)
    {
        LockGuardType guard(m_mutex);
        m_lastError = error; 
    }

    const std::string& VAT_Context::GetLastError() const
    {
        return m_lastError; 
    }

    void VAT_Context::SetStopRequested(bool stop)
    {
        LockGuardType guard(m_mutex);
        m_isStopRequested = stop; 
    }

    bool VAT_Context::GetStopRequested() const
    {
        LockGuardType guard(m_mutex);
        return m_isStopRequested; 
    }

    bool VAT_Context::ExecuteVisionCommand(VatCommand cmd)
    {
        LockGuardType guard(m_mutex);

        if (!m_processor)
            return false;

        StringMap params = m_params.visionParams;
        bool ret = false;

        switch (cmd)
        {
        case Measure:       ret = m_processor->RequestMeasureAsync(params);     break;
        case SetCok:        ret = m_processor->RequestSetCokAsync(params);      break;
        case InspReady:     ret = m_processor->RequestInspReadyAsync(params);   break;
        case DeviceCheck:   ret = m_processor->RequestDeviceCheckAsync(params); break;
        case Light:         ret = m_processor->RequestLightAsync(params);       break;
        }

        return ret;
    }

    void VAT_Context::SetVatParams(const VatParams& params)
    {
        LockGuardType guard(m_mutex);
        m_params = params;
    }

    std::string VAT_Context::GetSeqParam(const std::string& key) const
    {
        LockGuardType guard(m_mutex);
        auto it = m_params.seqParams.find(key);
        if (it != m_params.seqParams.end())
            return it->second;
        return std::string();
    }

    std::string VAT_Context::GetVisionParam(const std::string& key) const
    {
        LockGuardType guard(m_mutex);
        auto it = m_params.visionParams.find(key);
        if (it != m_params.visionParams.end())
            return it->second;
        return std::string();
    }

    void VAT_Context::SetSeqParam(const std::string& key, int value)
    {
        SetSeqParamAs<int>(key, value);
    }

    std::vector<VisionPosition> VAT_Context::GetVisionPositions() const
    {
        LockGuardType guard(m_mutex);
        return m_params.visionPositions;
    }

    bool VAT_Context::PopVisionPosition(VisionPosition& outPos)
    {
        LockGuardType guard(m_mutex);
        if (m_params.visionPositions.empty())
            return false;

        outPos = m_params.visionPositions.front();
        m_params.visionPositions.erase(m_params.visionPositions.begin());
        return true;
    }

    bool VAT_Context::PeekVisionPosition(VisionPosition& outPos)
    {
        LockGuardType guard(m_mutex);

        if (m_params.visionPositions.empty())
            return false;

        outPos = m_params.visionPositions.back();
        return true;
    }

    void VAT_Context::AddVisionPosition(const VisionPosition& pos)
    {
        LockGuardType guard(m_mutex);
        m_params.visionPositions.push_back(pos);
    }

    bool VAT_Context::IsVisionPositionEmpty() const
    {
        LockGuardType guard(m_mutex);
        return m_params.visionPositions.empty();
    }

    void VAT_Context::SetDataRepository(DataRepositoryPtr repo)
    {
        LockGuardType guard(m_mutex);
        m_repo = repo;
    }

    DataRepositoryPtr VAT_Context::getRepository() const
    {
        LockGuardType guard(m_mutex);
        return m_repo;
    }
}
