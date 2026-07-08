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
		// --- IComponentSetup 재정의 ---
		DataRepositoryPtr CreateRepository() override;
		VisionProcessorPtr CreateVisionProcessor() override;

		// --- ISequenceSetup 재정의 ---
        std::string GetSequenceName() const override;
        SequenceBuilderPtr CreateBuilder() override;
        void ConfigureParams(VMF::VisionContextPtr context) override;
	};
}





