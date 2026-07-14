#pragma once
#include "ComponentSetupBase.h"
#include "SqliteDataRepository.h"


namespace VMF_PLUGIN
{
	class C6SideInspectionStrategy : public VMF::ComponentSetupBase
	{
	public:
        std::string GetSequenceName() const override;
		
        VMF::DataRepositoryPtr CreateRepository() override;
		

        VMF::VisionProcessorPtr CreateVisionProcessor() override;

        VMF::SequenceBuilderPtr CreateBuilder() override;

        void ConfigureParams(VMF::VisionContextPtr ctx) override;
	};

} // namespace VMF_6SIDE
