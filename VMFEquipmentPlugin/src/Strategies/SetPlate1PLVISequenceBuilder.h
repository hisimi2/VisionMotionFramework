#pragma once

#include "SequenceBuilderBase.h"
#include "VMFEquipmentPluginExport.h"
#include "Types.h"
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
