#pragma once
#include "DefaultSetupStrategy.h"
#include "SqliteDataRepository.h"
#include <string>

namespace VMF_PLUGIN
{
	class C6SideInspectionStrategy : public VMF::DefaultSetupStrategy
	{
	public:
		std::string GetSequenceName() const override;

		VMF::DataRepositoryPtr CreateRepository() override;

		VMF::VisionProcessorPtr CreateVisionProcessor() override;

		VMF::SequenceBuilderPtr CreateBuilder() override;

		void ConfigureParams(VMF::VisionContextPtr ctx) override;
	};

} // namespace VMF_PLUGIN
