#include "pch.h"
#include "C6SideInspectionStrategy.h"
#include "scr\Protocol\VisionSixSideProcessor.h"
#include "C6SideInspectionSequenceBuilder.h"

using namespace VMF_PLUGIN;

std::string C6SideInspectionStrategy::GetSequenceName() const 
{
    return "6SideInspection";
}

VMF::DataRepositoryPtr C6SideInspectionStrategy::CreateRepository() 
{
    auto repo = std::make_shared<VMF::SqliteDataRepository>(
        "Data\\6SIDE_DATABASE.db", "Data\\Images");
    repo->Initialize();
    return repo;
}

VMF::VisionProcessorPtr C6SideInspectionStrategy::CreateVisionProcessor() 
{
    VMF::VisionConnectionConfig config("127.0.0.1", 8001, 3000);
    auto vm = std::make_shared<VMF::VisionSixSideProcessor>();
    vm->Initialize(config);
    return vm;
}

VMF::SequenceBuilderPtr C6SideInspectionStrategy::CreateBuilder() 
{
    return std::make_shared<
        C6SideInspectionSequenceBuilder>();
}

void C6SideInspectionStrategy::ConfigureParams(VMF::VisionContextPtr ctx)
{
    // Task별 파라미터는 Builder에서 SetTaskParams()로 설정하므로
    // Strategy의 ConfigureParams는 Repository 초기화 등 공통 작업만 수행
    (void)ctx;
}
