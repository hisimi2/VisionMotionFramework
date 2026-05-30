#pragma once
#include "EC_API.h"
#include "Context.h"
#include "Sequence.h"

namespace EC
{
    class EC_API SequenceBuilderBase
    {
    public:
        SequenceBuilderBase();
        virtual ~SequenceBuilderBase();
        SequencePtr CreateSequence(std::string Name);

    protected:
        virtual SequencePtr BuildSequence(std::string Name) = 0;
    };

    using SequenceBuilderPtr = std::shared_ptr<SequenceBuilderBase>;
}
