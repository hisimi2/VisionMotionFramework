#pragma once

#include "VisionMotionFramework\SequenceBuilderBase.h"
#include "PlugInAPI.h"
#include "VisionMotionFramework\Types.h"
#include "SetPlate1PLVITaskParamKeys.h"

#include <memory>

namespace VMF_PLUGIN
{
    // 전방 선언
    class SetPlate1PLVIStrategy;

    class SetPlate1PLVISequenceBuilder : public VMF::SequenceBuilderBase
    {
    protected:
        VMF::SequencePtr BuildSequence(const std::string& sequenceName) override;
    };
}
