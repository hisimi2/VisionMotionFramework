#pragma once
#include "SequenceSetupBase.h"
#include "SqliteDataRepository.h"

#include "VMFComposition/VisionProtocal/VisionMemoryProcessor.h"
#include "VMFComposition/Mock/CMockDataRepository.h"
#include "VMFComposition/Mock/CMockVisionEventHandler.h"

#include <memory>

/// <summary>
/// 메모리 기반 시퀀스 전략을 위한 기본 클래스입니다.
/// SequenceSetupBase를 상속받아 CreateRepository, CreateVisionProcessor를 구현합니다.
/// SetParam/AddVisionPoint 헬퍼는 ComponentSetupBase에서 상속받아 사용합니다.
/// </summary>
class MemorySequenceStrategy : public VMF::SequenceSetupBase
{
public:
    VMF::DataRepositoryPtr CreateRepository() override
    {
        // auto repo = std::make_shared<VMF::CMockDataRepository>();
        auto repo = std::make_shared<VMF::SqliteDataRepository>("Data\\VAT_DATABASE.db", "Data\\Images");
        repo->Initialize();

        return repo;
    }

    VMF::VisionEventHandlerPtr CreateVisionProcessor() override
    {
        VMF::VisionConnectionConfig config("127.0.0.1", 8080, 3000);
        auto vm = std::make_shared<VMF::CMockVisionEventHandler>();
        // auto vm = std::make_shared<VMF::VisionMemoryProcessor>();
        vm->Initialize(config);

        return vm;
    }

    // ISequenceSetup 순수가상: 파생 클래스에서 구현
    // std::string GetSequenceName() const override = 0;
    // SequenceBuilderPtr CreateBuilder() override = 0;

    // IComponentSetup 순수가상: ConfigreParams는 파생 클래스에서 구현
    // void ConfigureParams(VMF::VisionContextPtr ctx) override = 0;
};





