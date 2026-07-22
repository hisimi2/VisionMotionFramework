#pragma once
#include "DefaultSetupStrategy.h"
#include "VMFEquipmentPluginExport.h"

namespace VMF_PLUGIN
{
	class VMF_PLUGIN_API CSixSideInspectionStrategy : public VMF::DefaultSetupStrategy
	{
	public:
		std::string GetSequenceName() const override;

		VMF::DataRepositoryPtr CreateRepository() override;

		VMF::VisionProcessorPtr CreateVisionProcessor() override;

		VMF::SequenceBuilderPtr CreateBuilder() override;

		void ConfigureParams(VMF::VisionContextPtr ctx) override;
	};

} // namespace VMF_PLUGIN
