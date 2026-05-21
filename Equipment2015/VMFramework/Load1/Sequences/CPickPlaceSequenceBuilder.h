#pragma once

#include "SequenceBuilderBase.h"

namespace VMF_Load1
{
    namespace Sequence
    {
        class CPickPlaceSequenceBuilder : public VMF::SequenceBuilderBase
        {
        protected:
            VMF::SequencePtr BuildSequence(std::string sequenceName) override;
        };
    }
}
