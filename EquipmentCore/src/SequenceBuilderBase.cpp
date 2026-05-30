#include "StdAfx.h"

#include "Context.h"
#include "SequenceBuilderBase.h"
#include "Sequence.h"
#include "ISequence.h"

#include <string>
#include <memory> 
#include <direct.h>

namespace EC
{
    SequenceBuilderBase::SequenceBuilderBase() {}
    SequenceBuilderBase::~SequenceBuilderBase() {}

    std::unique_ptr<ISequence> SequenceBuilderBase::CreateSequence(std::string Name)
    {
        return BuildSequence(Name);
    }
} 
