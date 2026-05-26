#pragma once

#include "SequenceBuilderBase.h"

namespace AutoThread
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
