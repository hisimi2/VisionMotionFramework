#pragma once

#include "DefaultSetupStrategy.h"
#include "SampleZFocusSequenceBuilder.h"
#include "VMFEquipmentPluginExport.h"

namespace VMF_Sample
{
	using namespace VMF;

	class VMF_PLUGIN_API SampleSequenceStrategy : public DefaultSetupStrategy
	{
	public:
        std::string GetSequenceName() const override;
        SequenceBuilderPtr CreateBuilder() override;
        void ConfigureParams(VMF::VisionContextPtr context) override;
	};
}





