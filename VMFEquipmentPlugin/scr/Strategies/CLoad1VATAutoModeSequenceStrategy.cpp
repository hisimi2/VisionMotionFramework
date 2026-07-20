#include "pch.h"
#include "CLoad1VATAutoModeSequenceStrategy.h"
#include "CLoad1VATAutoModeSequenceBuilder.h"

using namespace VMF_PLUGIN;


VMF::SequenceBuilderPtr CLoad1VATAutoModeSequenceStrategy::CreateBuilder() 
{
    return std::make_shared<CLoad1VATAutoModeSequenceBuilder>();
}

void CLoad1VATAutoModeSequenceStrategy::ConfigureParams(VMF::VisionContextPtr ctx)
{
    // Task별 파라미터는 Builder에서 SetTaskParams()로 직접 주입
    // ConfigureParams는 Repository 초기화 등 공통 작업만 수행
    (void)ctx;
}
