#pragma once
#include "CLoad1VATBaseSequenceStrategy.h"


namespace VMF_PLUGIN
{

	class CLoad1VATPlateJigDetachSequenceStrategy : public CLoad1VATBaseSequenceStrategy
	{
	public:
		std::string GetSequenceName() const override { return "Load1PlateJigDetach"; }

        VMF::SequenceBuilderPtr CreateBuilder() override;

        void ConfigureParams(VMF::VisionContextPtr ctx) override;
	};

} 
