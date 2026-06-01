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

        // 1. 실행할 시퀀스 이름 반환
        virtual std::string GetActivityName() const = 0;

        // 2. 사용할 ActivityBuilder 생성
        virtual ActivityBuilderPtr CreateBuilder() = 0;

        // 3. 실행 파라미터 설정 (Context에 주입)
        virtual void ConfigureParams(ContextPtr context) = 0;
      
    };
    
    using ActivityStrategyPtr = std::shared_ptr<IActivityStrategy>;
}
