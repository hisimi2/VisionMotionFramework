#include "StdAfx.h"

#include "Context.h"
#include "SequenceBuilderBase.h"
#include "Sequence.h"
#include "ISequence.h"


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

std::unique_ptr<ISequence> SequenceBuilderBase::CreateSequence(const std::string& sequenceName)
    {
        return BuildSequence(sequenceName);
    }

    void SequenceBuilderBase::SetTaskParamsMap(const std::unordered_map<std::string, VisionParams>& taskParamsMap)
    {
        m_taskParamsMap = taskParamsMap;
    }
} // namespace VMF
