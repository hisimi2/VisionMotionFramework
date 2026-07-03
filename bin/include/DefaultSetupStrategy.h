#pragma once

#include "ComponentSetupBase.h"
#include "SqliteDataRepository.h"
#include "Mock/CMockVisionEventHandler.h"

#include <memory>

/// <summary>
/// 기본 컴포넌트 설정 전략 클래스.
/// ComponentSetupBase를 상속받아 CreateRepository, CreateVisionProcessor의 기본 구현을 제공합니다.
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
class VMF_API DefaultSetupStrategy : public VMF::ComponentSetupBase
{
public:
DefaultSetupStrategy();
    ~DefaultSetupStrategy() override;

    VMF::DataRepositoryPtr CreateRepository() override;
    VMF::VisionProcessorPtr CreateVisionProcessor() override;
};
