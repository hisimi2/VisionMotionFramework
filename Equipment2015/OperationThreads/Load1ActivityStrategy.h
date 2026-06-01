#pragma once
#
#include "ActivityStrategyBase.h"
#include "Load1ActivityBuilder.h"

namespace OperationThread
{
    using namespace EC;
    class Load1ActivityStrategy : public ActivityStrategyBase
    {
    public:

        ActivityBuilderPtr CreateBuilder()
        {
            return std::make_shared<CLoad1ActivityBuilder>();
        }

        void ConfigureParams(ContextPtr ctx)
        {
            ctx->SetParamAs<int>("repeatCount", 2);
            ctx->SetParamAs<int>("timeout", 3000);
        }
    };
}

