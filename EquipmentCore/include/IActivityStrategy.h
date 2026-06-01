#pragma once

#include <string>
#include <memory>
#include "Context.h"
#include "ActivityBuilderBase.h"

namespace EC
{
    class IActivityStrategy
    {
    public:
        virtual ~IActivityStrategy() = default;

        // 1. 사용할 ActivityBuilder 생성
        virtual ActivityBuilderPtr CreateBuilder() = 0;

        // 2. 실행 파라미터 설정 (Context에 주입)
        virtual void ConfigureParams(ContextPtr context) = 0;
      
    };
    
    using ActivityStrategyPtr = std::shared_ptr<IActivityStrategy>;
}
