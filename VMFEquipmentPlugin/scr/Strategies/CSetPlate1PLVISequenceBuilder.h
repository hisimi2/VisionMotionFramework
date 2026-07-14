#pragma once
#include "SequenceBuilderBase.h"


namespace VMF_PLUGIN
{
	class CSetPlate1PLVISequenceBuilder : public VMF::SequenceBuilderBase
	{
	protected:
        VMF::SequencePtr BuildSequence(
            const std::string& sequenceName) override;
		
	};
} // namespace VMF_PLUGIN
