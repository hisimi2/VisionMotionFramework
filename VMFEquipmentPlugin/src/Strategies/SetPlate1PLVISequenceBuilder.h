#pragma once
#include "SequenceBuilderBase.h"
#include "VMFEquipmentPluginExport.h"


namespace VMF_PLUGIN
{
	class VMF_PLUGIN_API SetPlate1PLVISequenceBuilder : public VMF::SequenceBuilderBase
	{
	protected:
        VMF::SequencePtr BuildSequence(
            const std::string& sequenceName) override;
		
	};
} // namespace VMF_PLUGIN
