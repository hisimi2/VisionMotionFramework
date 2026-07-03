#include "StdAfx.h"

#include "Context.h"
#include "SequenceBuilderBase.h"
#include "Sequence.h"
#include "ISequence.h"


#include <string>
#include <memory> 
#include <direct.h>
#include <sstream>

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

    // ============================================================================
    // VisionParams 헬퍼 메서드 구현
    // ============================================================================

    void SequenceBuilderBase::SetParam(VisionParams& params, const std::string& key, const std::string& value)
    {
        params.visionParams[key] = value;
    }

    void SequenceBuilderBase::SetParam(VisionParams& params, const std::string& key, double value)
    {
        std::ostringstream oss;
        oss << value;
        params.visionParams[key] = oss.str();
    }

    void SequenceBuilderBase::AddVisionPoint(VisionParams& params, int locateId, int requestId, double x, double y, double z)
    {
        std::vector<double> pos;
        pos.push_back(x);
        pos.push_back(y);
        pos.push_back(z);
        params.visionPositions.push_back(VisionPosition(pos, locateId, requestId));
    }

    void SequenceBuilderBase::AddVisionPoint(VisionParams& params, int locateId, int requestId, double x, double y, double z, double t1, double t2)
    {
        std::vector<double> pos;
        pos.push_back(x);
        pos.push_back(y);
        pos.push_back(z);
        pos.push_back(t1);
        pos.push_back(t2);
        params.visionPositions.push_back(VisionPosition(pos, locateId, requestId));
    }
} // namespace VMF
