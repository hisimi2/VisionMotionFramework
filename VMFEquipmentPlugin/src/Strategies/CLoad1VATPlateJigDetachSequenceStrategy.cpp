#include "pch.h"
#include "CLoad1VATPlateJigDetachSequenceStrategy.h"
#include "CLoad1VATPlateJigDetachSequenceBuilder.h"


using namespace VMF;
using namespace VMF_PLUGIN;


VMF::SequenceBuilderPtr CLoad1VATPlateJigDetachSequenceStrategy::CreateBuilder() 
{
    return std::make_shared<CLoad1VATPlateJigDetachSequenceBuilder>();
}

void CLoad1VATPlateJigDetachSequenceStrategy::ConfigureParams(VMF::VisionContextPtr ctx) 
{
    // Task별 파라미터는 Builder에서 SetTaskParams()로 직접 주입
    (void)ctx;
}
