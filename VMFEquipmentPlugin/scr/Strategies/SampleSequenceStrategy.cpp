#include "pch.h"
#include "SampleSequenceStrategy.h"
#include "SampleZFocusSequenceBuilder.h"

namespace VMF_Sample
{
    std::string SampleSequenceStrategy::GetSequenceName() const
    {
        return "SampleZFocus";
    }

    SequenceBuilderPtr SampleSequenceStrategy::CreateBuilder()
    {
        return std::make_shared<SampleZFocusSequenceBuilder>();
    }

    void SampleSequenceStrategy::ConfigureParams(VMF::VisionContextPtr context) 
    {
        if (!context)
            return;

        // Example: Set default camera index, package ID, etc.
        // context->SetSeqParam("CameraIndex", "6");
        // context->SetSeqParam("PkgID", "1");
        // context->SetSeqParam("TimeOutMs", "10000");

        // !!! Modification Required: Add equipment-specific preset parameters here !!!
    }
}

