#pragma once

#include "VisionMotionFramework\SequenceBuilderBase.h"
#include "VMFEquipmentPluginExport.h"
#include "VisionMotionFramework\Types.h"
#include "ParamKeys.h"

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
