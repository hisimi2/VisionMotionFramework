#pragma once
#include "EC_API.h"
#include "ISequenceStrategy.h"
#include "SequenceBuilderBase.h"
#include "Context.h"

#include <string>

namespace EC
{
    class EC_API SequenceStrategyBase : public ISequenceStrategy
    {

    public:
        SequenceStrategyBase();
        ~SequenceStrategyBase() override = default;
        std::string GetSequenceName() const override = 0;
        SequenceBuilderPtr CreateBuilder() override = 0;
        void ConfigureParams(ContextPtr context) override = 0;
    };
}
