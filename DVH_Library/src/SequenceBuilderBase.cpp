#include "StdAfx.h"

#include "Vat_context.h"
#include "SequenceBuilderBase.h"
#include "VatSequence.h"
#include "IVatSequence.h"

#include "SqliteDataRepository.h"
#include "AsyncDataRepository.h"
#include "CompatUtils.h"

#include <string>
#include <boost/make_shared.hpp>
#include <direct.h>

namespace DVH_VAT
{
    SequenceBuilderBase::SequenceBuilderBase() {}
    SequenceBuilderBase::~SequenceBuilderBase() {}

    static void EnsureDirExists(const std::string& path) 
    {
        _mkdir(path.c_str());
    }

    boost::unique_ptr<IVatSequence> SequenceBuilderBase::CreateSequence(std::string sequenceName)
    {
        return BuildSequence(sequenceName);
    }

} // namespace DVH_VAT
