#include "stdafx.h"
#include "DefaultSetupStrategy.h"
#include "ConnectionManager.h"
#include "SqliteDataRepository.h"
#include "Mock/CMockVisionEventHandler.h"

namespace VMF
{
    DefaultSetupStrategy::DefaultSetupStrategy()
        : m_adapter(nullptr)
    {
    }

    void DefaultSetupStrategy::SetActuator(IActuator* adapter)
    {
        m_adapter = adapter;
    }

    IActuator* DefaultSetupStrategy::GetActuator()
    {
        return m_adapter;
    }

    void DefaultSetupStrategy::SetConnectionConfig(const VisionConnectionConfig& config)
    {
        m_connectionConfig = config;
    }

    const VisionConnectionConfig& DefaultSetupStrategy::GetConnectionConfig() const
    {
        return m_connectionConfig;
    }

    bool DefaultSetupStrategy::IsUsingConnectionManager() const
    {
        return !m_connectionConfig.address.empty() && m_connectionConfig.port > 0;
    }

    std::shared_ptr<VC::Controller> DefaultSetupStrategy::GetOrCreateSharedController()
    {
        if (!IsUsingConnectionManager())
            return nullptr;

        return ConnectionManager::GetInstance().GetOrCreateConnection(
            m_connectionConfig.address,
            m_connectionConfig.port,
            m_connectionConfig.timeoutMs);
    }

    void DefaultSetupStrategy::ConfigureParams(VMF::VisionContextPtr /*context*/)
    {
        // 기본 구현: 아무 동작도 하지 않음
        // 파생 클래스에서 필요한 경우 오버라이드하여 사용
    }

    DataRepositoryPtr DefaultSetupStrategy::CreateRepository()
    {
        auto repo = std::make_shared<SqliteDataRepository>("Data\\VAT_DATABASE.db", "Data\\Images");
        repo->Initialize();
        return repo;
    }

    VisionProcessorPtr DefaultSetupStrategy::CreateVision()
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
        VisionConnectionConfig localConfig = GetConnectionConfig();
        if (localConfig.address.empty() || localConfig.port == 0)
        {
            localConfig = VisionConnectionConfig("127.0.0.1", 8080, 3000);
        }
        auto vm = std::make_shared<CMockVisionEventHandler>();
        vm->Initialize(localConfig);

        return vm;
    }
}
