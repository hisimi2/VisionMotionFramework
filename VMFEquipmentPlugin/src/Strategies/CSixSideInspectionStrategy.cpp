#include "pch.h"
#include "CSixSideInspectionStrategy.h"
#include "SqliteDataRepository.h"
#include "scr\Protocol\VisionSixSideProcessor.h"
#include "CSixSideInspectionSequenceBuilder.h"

using namespace VMF;
using namespace VMF_PLUGIN;

std::string CSixSideInspectionStrategy::GetSequenceName() const 
{
    return "6SideInspection";
}

VMF::DataRepositoryPtr CSixSideInspectionStrategy::CreateRepository() 
{
    auto repo = std::make_shared<VMF::SqliteDataRepository>(
        "Data\\6SIDE_DATABASE.db", "Data\\Images");
    repo->Initialize();
    return repo; 
}

VMF::VisionProcessorPtr CSixSideInspectionStrategy::CreateVisionProcessor() 
{
    VMF::VisionConnectionConfig config("127.0.0.1", 8001, 3000);
    auto vm = std::make_shared<VisionSixSideProcessor>();
    vm->Initialize(config);
    return vm;
}

VMF::SequenceBuilderPtr CSixSideInspectionStrategy::CreateBuilder() 
{
    return std::make_shared<
        CSixSideInspectionSequenceBuilder>();
}

void CSixSideInspectionStrategy::ConfigureParams(VMF::VisionContextPtr ctx)
{
    // Task별 파라미터는 Builder에서 SetTaskParams()로 설정하므로
    // Strategy의 ConfigureParams는 Repository 초기화 등 공통 작업만 수행
    (void)ctx;
}
