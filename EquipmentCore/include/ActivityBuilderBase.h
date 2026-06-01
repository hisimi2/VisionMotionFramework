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
        ActivityPtr Create(std::string Name);

    protected:
        virtual ActivityPtr Build(std::string Name) = 0;
    };

    using ActivityBuilderPtr = std::shared_ptr<ActivityBuilderBase>;
}
