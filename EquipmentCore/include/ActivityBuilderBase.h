#pragma once
#include "EC_API.h"
#include <memory>

namespace EC
{
    class Context;
    class IActivity;

    using ContextPtr = std::shared_ptr<Context>;
    using ActivityPtr = std::unique_ptr<IActivity>;

    class EC_API ActivityBuilderBase 
    {
    public:
        ActivityBuilderBase() = default;
        virtual ~ActivityBuilderBase() = default;
        ActivityPtr Create();
        virtual void ConfigureParams(ContextPtr context) = 0;

    protected:
        virtual ActivityPtr Build() = 0;
    };

    using ActivityBuilderPtr = std::shared_ptr<ActivityBuilderBase>;
}
