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
        SequencePtr CreateSequence(std::string sequenceName);

    protected:
        virtual SequencePtr BuildSequence(std::string sequenceName) = 0;
    };

    using SequenceBuilderPtr = std::shared_ptr<SequenceBuilderBase>;
}
