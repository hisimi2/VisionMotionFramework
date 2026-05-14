#include "StdAfx.h"

#include "Context.h"
#include "SequenceBuilderBase.h"
#include "Sequence.h"
#include "ISequence.h"

#include "SqliteDataRepository.h"
#include "AsyncDataRepository.h"
#include "CompatUtils.h"

#include <string>
#include <memory> 
#include <direct.h>

namespace VMF
{
    SequenceBuilderBase::SequenceBuilderBase() {}
    SequenceBuilderBase::~SequenceBuilderBase() {}

    static void EnsureDirExists(const std::string& path) 
    {
        _mkdir(path.c_str());
    }

    std::unique_ptr<ISequence> SequenceBuilderBase::CreateSequence(std::string sequenceName)
    {
        return BuildSequence(sequenceName);
    }
} // namespace VMF
