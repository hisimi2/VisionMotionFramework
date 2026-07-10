#pragma once
#include "DefaultSetupStrategy.h"
#include "VMFEquipmentPluginExport.h"

namespace VMF_Sample
{
    using namespace VMF;

    class VMF_PLUGIN_API PLVILoad1Strategy : public VMF::DefaultSetupStrategy
    {
    public:
        PLVILoad1Strategy();
        ~PLVILoad1Strategy();

        // --- ISequenceSetup 재정의 (Direct Mode에서도 필요) ---
        std::string GetSequenceName() const override;
        SequenceBuilderPtr CreateBuilder() override;
    };
}

