#pragma once
#include "EC_API.h"
#include "IActivityStrategy.h"
#include "ActivityBuilderBase.h"
#include "Context.h"

#include <string>

namespace EC
{
    class EC_API ActivityStrategyBase : public IActivityStrategy
    {

    public:
        ActivityStrategyBase();
        ~ActivityStrategyBase() override = default;

        std::string GetActivityName() const override = 0;
        ActivityBuilderPtr CreateBuilder() override = 0;
        void ConfigureParams(ContextPtr context) override = 0;
    };
}
