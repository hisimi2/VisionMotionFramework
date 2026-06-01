#pragma once
#include "EC_API.h"
#include "Activity.h"

namespace EC
{
    class EC_API ActivityBuilderBase
    {
    public:
        ActivityBuilderBase();
        virtual ~ActivityBuilderBase();
        ActivityPtr Create();

    protected:
        virtual ActivityPtr Build() = 0;
    };

    using ActivityBuilderPtr = std::shared_ptr<ActivityBuilderBase>;
}
