#pragma once
#include "CLoad1VATBaseSequenceStrategy.h"


#include "VMFEquipmentPluginExport.h"


namespace VMF_PLUGIN
{
	class VMF_PLUGIN_API CLoad1VATPlateJigAttachSequenceStrategy : public CLoad1VATBaseSequenceStrategy
	{
	public:
		std::string GetSequenceName() const override { return "Load1PlateJigAttach"; }

        VMF::SequenceBuilderPtr CreateBuilder() override;

        void ConfigureParams(VMF::VisionContextPtr ctx) override;
	};
}
