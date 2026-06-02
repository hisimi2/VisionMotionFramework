#pragma once
#include "EC_API.h"
#include "Context.h"
#include "Activity.h"

namespace EC
{
    class EC_API ActivityBuilderBase 
    {
    public:
        ActivityBuilderBase();
        virtual ~ActivityBuilderBase();
        ActivityPtr Create();
        virtual void ConfigureParams(ContextPtr context) = 0;

    protected:
        virtual ActivityPtr Build() = 0;
    };

    using ActivityBuilderPtr = std::shared_ptr<ActivityBuilderBase>;
}
