#include "pch.h"
#include "SetPlate1PLVIStrategy.h"
#include "SqliteDataRepository.h"
#include "..\Protocol\VisionPlviProcessor.h"
#include "..\Strategies\SetPlate1PLVISequenceBuilder.h"
#include <sstream>
#include <vector>

using namespace VMF;
using namespace VMF_PLUGIN;

std::string SetPlate1PLVIStrategy::GetSequenceName() const
{
    return "SetPlate1PLVI";
}

VMF::DataRepositoryPtr SetPlate1PLVIStrategy::CreateRepository()
{
    auto repo = std::make_shared<VMF::SqliteDataRepository>(
        "Data\\PLVI_DATABASE.db", "Data\\Images");
    repo->Initialize();
    return repo;
}

VMF::VisionProcessorPtr SetPlate1PLVIStrategy::CreateVisionProcessor()
{
    VMF::VisionConnectionConfig config("127.0.0.1", 8000, 3000);
    auto vm = std::make_shared<VisionPlviProcessor>();
    vm->Initialize(config);
    return vm;
}

VMF::SequenceBuilderPtr SetPlate1PLVIStrategy::CreateBuilder()
{
    return std::make_shared<VMF_PLUGIN::SetPlate1PLVISequenceBuilder>();
}

void SetPlate1PLVIStrategy::ConfigureParams(VMF::VisionContextPtr ctx)
{
    // Note: Execution parameters are now configured in SetPlate1PLVISequenceBuilder::ConfigureContext()
    // This method is kept for interface compliance but does not set execution parameters.
    // If repository persistence is needed, it should be done here.
    (void)ctx;
}

VMF::StringMap SetPlate1PLVIStrategy::GetVisionParams(const std::string& presetName) const
{
    VMF::StringMap p;
    return VMF::StringMap();
}


