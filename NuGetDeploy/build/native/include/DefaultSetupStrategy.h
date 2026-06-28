#pragma once

#include "ComponentSetupBase.h"
#include "SqliteDataRepository.h"
#include "Mock/CMockVisionEventHandler.h"

#include <memory>

/// <summary>
/// 기본 컴포넌트 설정 전략 클래스.
/// ComponentSetupBase를 상속받아 CreateRepository, CreateVisionProcessor의 기본 구현을 제공합니다.
/// SetParam/AddVisionPoint 헬퍼는 ComponentSetupBase에서 상속받아 사용합니다.
/// 
/// 다중 서버 대응:
/// SetConnectionConfig()를 통해 Vision 서버 연결 설정이 주입된 경우,
/// ConnectionManager를 통해 단일 소켓 연결을 공유합니다.
/// 
/// !!! 파생 클래스에서 구현해야 하는 순수 가상 함수 !!!
/// - GetSequenceName()  — ISequenceSetup
/// - CreateBuilder()    — ISequenceSetup
/// - ConfigureParams()  — IComponentSetup
/// 
/// 사용 예 (파생 클래스):
///   class MyStrategy : public DefaultSetupStrategy
///   {
///       std::string GetSequenceName() const override { return "MySequence"; }
///       SequenceBuilderPtr CreateBuilder() override { return std::make_shared<MyBuilder>(); }
///       void ConfigureParams(VMF::VisionContextPtr ctx) override { ... }
///   };
/// </summary>
class DefaultSetupStrategy : public VMF::ComponentSetupBase
{
public:
    VMF::DataRepositoryPtr CreateRepository() override
    {
        auto repo = std::make_shared<VMF::SqliteDataRepository>("Data\\VAT_DATABASE.db", "Data\\Images");
        repo->Initialize();
        return repo;
    }

    VMF::VisionProcessorPtr CreateVisionProcessor() override
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
};
