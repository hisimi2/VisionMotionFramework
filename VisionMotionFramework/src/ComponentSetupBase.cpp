#include "stdafx.h"
#include "ComponentSetupBase.h"
#include "ConnectionManager.h"
#include "SqliteDataRepository.h"
#include "Mock/CMockVisionEventHandler.h"

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

    void ComponentSetupBase::ConfigureParams(VMF::VisionContextPtr /*context*/)
    {
        // 기본 구현: 아무 동작도 하지 않음
        // 파생 클래스에서 필요한 경우 오버라이드하여 사용
    }

    DataRepositoryPtr ComponentSetupBase::CreateRepository()
    {
        auto repo = std::make_shared<SqliteDataRepository>("Data\\VAT_DATABASE.db", "Data\\Images");
        repo->Initialize();
        return repo;
    }

    VisionProcessorPtr ComponentSetupBase::CreateVisionProcessor()
    {
        // [ConnectionManager 모드]
        // SetConnectionConfig()로 연결 설정이 주입되면 ConnectionManager를 통해
        // 공유 Controller를 사용하여 단일 소켓 연결을 유지합니다.
        if (IsUsingConnectionManager())
        {
            auto sharedCtrl = GetOrCreateSharedController();
            if (sharedCtrl)
            {
                auto vm = std::make_shared<CMockVisionEventHandler>();
                VC::Status status = vm->InitializeWithSharedController(
                    sharedCtrl, GetConnectionConfig());

                if (status == VC::VisionOK)
                {
                    return vm;
                }
            }
            // ConnectionManager 실패 시 기본 방식으로 fallback
        }

// [기본 모드] - 기존 방식: 직접 연결 생성
        // 생성자에서 주입된 ConnectionConfig 우선 사용, 없으면 기본값
        VisionConnectionConfig config = GetConnectionConfig();
        if (config.address.empty() || config.port == 0)
        {
            config = VisionConnectionConfig("127.0.0.1", 8080, 3000);
        }
        auto vm = std::make_shared<CMockVisionEventHandler>();
        vm->Initialize(config);

        return vm;
    }
}
