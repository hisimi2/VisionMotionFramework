#pragma once
#include "DefaultSetupStrategy.h"
#include "VMFEquipmentPluginExport.h"

namespace VMF_Sample
{
    using namespace VMF;

    class PLVILoad1Strategy : public VMF::DefaultSetupStrategy
    {
    public:
        PLVILoad1Strategy();
        ~PLVILoad1Strategy();
    };
}
