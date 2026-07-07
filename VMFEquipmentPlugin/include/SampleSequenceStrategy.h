#pragma once

// DefaultSetupStrategy is not DLL-exported, so suppress C4275 for derived class.
#pragma warning(push)
#pragma warning(disable : 4275)

#include "DefaultSetupStrategy.h"
#include "SampleZFocusSequenceBuilder.h"
#include "VMFEquipmentPluginExport.h"

namespace VMF_Sample
{
	using namespace VMF;
	using namespace Sequence;

	class VMFEQUIPMENTPLUGIN_API SampleSequenceStrategy : public DefaultSetupStrategy
	{
	public:
        std::string GetSequenceName() const override;
        SequenceBuilderPtr CreateBuilder() override;
        void ConfigureParams(VMF::VisionContextPtr context) override;
	};
} // namespace VMF_Sample

#pragma warning(pop)
