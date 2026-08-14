#pragma once

#include "SequenceBuilderBase.h"
#include "VMFEquipmentPluginExport.h"
#include "Types.h"
#include "ParamKeys.h"

#include <memory>

namespace VMF_PLUGIN
{
    class SetPlate1PLVISequenceBuilder : public VMF::SequenceBuilderBase
    {
    public:
        void SetTaskParams(const VMF::TaskParams& params);
        void ConfigureContext(VMF::Context& ctx) override;

    protected:
        VMF::SequencePtr BuildSequence(const std::string& sequenceName) override;

    private:
        VMF::TaskParams m_taskParams;
    };
}
