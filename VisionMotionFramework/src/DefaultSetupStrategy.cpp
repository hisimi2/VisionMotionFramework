#include "stdafx.h"
#include "DefaultSetupStrategy.h"
    
VMF::DataRepositoryPtr DefaultSetupStrategy::CreateRepository()
{
    auto repo = std::make_shared<VMF::SqliteDataRepository>("Data\\VAT_DATABASE.db", "Data\\Images");
    repo->Initialize(); 
    return repo;
}

VMF::VisionProcessorPtr DefaultSetupStrategy::CreateVisionProcessor()
{   
    // [ConnectionManager 모드]
    // SetConnectionConfig()로 연결 설정이 주입되면 ConnectionManager를 통해
    // 공유 Controller를 사용하여 단일 소켓 연결을 유지합니다.
    if (IsUsingConnectionManager())
    {
        auto sharedCtrl = GetOrCreateSharedController();
        if (sharedCtrl)
        {
            auto vm = std::make_shared<VMF::CMockVisionEventHandler>();
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
    VMF::VisionConnectionConfig config("127.0.0.1", 8080, 3000);
    auto vm = std::make_shared<VMF::CMockVisionEventHandler>();
    vm->Initialize(config);

    return vm;
}

DefaultSetupStrategy::DefaultSetupStrategy() = default;
DefaultSetupStrategy::~DefaultSetupStrategy() = default;
