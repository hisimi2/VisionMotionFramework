#pragma once

#include "SequenceBuilderBase.h"
#include "CLoad1PerformBacklashScanningTask.h"

namespace VAT_LOAD1
{
	namespace Sequence
	{
        using namespace VAT_LOAD1::Task;

		class CLoad1BacklashSequenceBuilder : public DVH_VAT::SequenceBuilderBase
		{
		protected:
            DVH_VAT::VatSequencePtr BuildSequence(std::string sequenceName) override
			{
                DVH_VAT::VatSequencePtr seq(new DVH_VAT::VatSequence(sequenceName));

				seq->AddTask(DVH_VAT::TaskStepPtr(new CLoad1PerformBacklashScanningTask()));

				return DVH_VAT::VatSequencePtr(seq.release());
			}
		};
	}
}
