#include "stdafx.h"
#include "ComponentSetupBase.h"
#include "ConnectionManager.h"

namespace VMF
{
    ComponentSetupBase::ComponentSetupBase()
        : m_adapter(nullptr)
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
    }

    const VisionConnectionConfig& ComponentSetupBase::GetConnectionConfig() const
    {
        return m_connectionConfig;
    }

    bool ComponentSetupBase::IsUsingConnectionManager() const
    {
        return !m_connectionConfig.address.empty() && m_connectionConfig.port > 0;
    }

    std::shared_ptr<VC::Controller> ComponentSetupBase::GetOrCreateSharedController()
    {
        if (!IsUsingConnectionManager())
            return nullptr;

        return ConnectionManager::GetInstance().GetOrCreateConnection(
            m_connectionConfig.address,
            m_connectionConfig.port,
            m_connectionConfig.timeoutMs);
    }
}