#include "StdAfx.h"

#include "VAT_Context.h"
#include "SequenceBuilderBase.h"
#include "VatSequence.h"
#include "IVatSequence.h"

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

    std::unique_ptr<IVatSequence> SequenceBuilderBase::CreateSequence(std::string sequenceName)
    {
        return BuildSequence(sequenceName);
    }
} // namespace VMF
