#pragma once

// DefaultSetupStrategy is not DLL-exported, so suppress C4275 for derived class.
#pragma warning(push)
#pragma warning(disable : 4275)

#include "DefaultSetupStrategy.h"
#include "Sequences/SampleZFocusSequenceBuilder.h"
#include "VMFEquipmentPluginExport.h"

namespace VMF_Sample
{
	using namespace VMF;
	using namespace Sequence;

	/// <summary>
	/// [Sample] Focus Check Sequence strategy class.
	/// Inherits DefaultSetupStrategy for basic CreateRepository/CreateVisionProcessor implementations.
	/// 
	/// [Responsibility Scope]
	/// - CreateRepository(): DB initialization (SqliteDataRepository) - uses DefaultSetupStrategy
	/// - CreateVisionProcessor(): Vision server connection and processor init - uses DefaultSetupStrategy
	/// - CreateBuilder(): Returns SampleZFocusSequenceBuilder
	/// 
	/// [Builder Responsibility Separation]
	/// +------------------------------------------+
	/// | SampleSequenceStrategy (Strategy)          |
	/// | + CreateRepository(): DB init              |
	/// | + CreateVisionProcessor(): Vision connect  |
	/// | + ConfigureParams(): Set context params    |
	/// | + CreateBuilder(): Return builder          |
	/// +------------------------------------------+
	/// +------------------------------------------+
	/// | SampleZFocusSequenceBuilder (Builder)     |
	/// | + BuildSequence(): Assemble tasks+params  |
	/// +------------------------------------------+
	/// 
	/// !!! Modification Guide !!!
	/// 1. GetSequenceName(): Return the sequence name to execute
	/// 2. CreateBuilder(): Replace with equipment-specific sequence builder
	/// 3. ConfigureParams(): Set equipment-specific preset parameters in Context
	/// </summary>
	class VMFEQUIPMENTPLUGIN_API SampleSequenceStrategy : public DefaultSetupStrategy
	{
	public:
		std::string GetSequenceName() const override { return "SampleZFocus"; }

		SequenceBuilderPtr CreateBuilder() override
		{
			return std::make_shared<SampleZFocusSequenceBuilder>();
		}

		/// <summary>
		/// Sets equipment-specific preset parameters in the Context.
		/// !!! Modification Required: Set parameters as needed for your equipment !!!
		/// </summary>
		void ConfigureParams(VMF::VisionContextPtr context) override
		{
			if (!context)
				return;

			// Example: Set default camera index, package ID, etc.
			// context->SetSeqParam("CameraIndex", "6");
			// context->SetSeqParam("PkgID", "1");
			// context->SetSeqParam("TimeOutMs", "10000");

			// !!! Modification Required: Add equipment-specific preset parameters here !!!
		}
	};
} // namespace VMF_Sample

#pragma warning(pop)
