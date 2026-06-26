#include "stdafx.h"
#include "ComponentSetupBase.h"

namespace VMF
{
    ComponentSetupBase::ComponentSetupBase()
        : m_adapter(nullptr)
        , m_useConnectionManager(false)
    {
    }

    void ComponentSetupBase::SetActuator(IActuator* adapter)
    {
        m_adapter = adapter;
    }

    IActuator* ComponentSetupBase::GetActuator()
    {
        return m_adapter;
    }

    void ComponentSetupBase::SetConnectionConfig(const VisionConnectionConfig& config)
    {
        m_connectionConfig = config;
        m_useConnectionManager = true;
    }

    const VisionConnectionConfig& ComponentSetupBase::GetConnectionConfig() const
    {
        return m_connectionConfig;
    }

    bool ComponentSetupBase::IsUsingConnectionManager() const
    {
        return m_useConnectionManager;
    }

    std::shared_ptr<VC::Controller> ComponentSetupBase::GetOrCreateSharedController()
    {
        if (!m_useConnectionManager)
        {
            return nullptr;
        }

        // ConnectionManager를 통해 공유 Controller 획득 또는 생성
        return ConnectionManager::GetInstance().GetOrCreateConnection(
            m_connectionConfig.address,
            m_connectionConfig.port,
            m_connectionConfig.timeoutMs);
    }

    void ComponentSetupBase::SetParam(VisionParams& params, const std::string& key, const std::string& value)
    {
        params.seqParams[key] = value;
    }

    void ComponentSetupBase::SetParam(VisionParams& params, const std::string& key, double value)
    {
        std::ostringstream oss;
        oss << value;
        params.seqParams[key] = oss.str();
    }

    void ComponentSetupBase::AddVisionPoint(VisionParams& params, int locateId, int requestId, double x, double y, double z)
    {
        std::vector<double> pos;
        pos.push_back(x);
        pos.push_back(y);
        pos.push_back(z);
        params.visionPositions.push_back(VisionPosition(pos, locateId, requestId));
    }

    void ComponentSetupBase::AddVisionPoint(VisionParams& params, int locateId, int requestId, double x, double y, double z, double t1, double t2)
    {
        std::vector<double> pos;
        pos.push_back(x);
        pos.push_back(y);
        pos.push_back(z);
        pos.push_back(t1);
        pos.push_back(t2);
        params.visionPositions.push_back(VisionPosition(pos, locateId, requestId));
    }
}