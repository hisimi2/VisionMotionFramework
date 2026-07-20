#include "pch.h"
#include "CLoad1VATPlateJigAttachSequenceStrategy.h"
#include "CLoad1VATPlateJigAttachSequenceBuilder.h"





using namespace VMF_PLUGIN;

VMF::SequenceBuilderPtr CLoad1VATPlateJigAttachSequenceStrategy::CreateBuilder() 
{
    return std::make_shared<CLoad1VATPlateJigAttachSequenceBuilder>();
}

void CLoad1VATPlateJigAttachSequenceStrategy::ConfigureParams(VMF::VisionContextPtr ctx) 
{
    // Task별 파라미터는 Builder에서 SetTaskParams()로 직접 주입
    (void)ctx;
}
