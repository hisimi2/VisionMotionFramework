#pragma once

#include "SequenceBuilderBase.h"
#include "CLoad1PerformBacklashScanningTask.h"

namespace VAT_LOAD1
{
	namespace Sequence
	{
        using namespace VAT_LOAD1::Task;

		class CLoad1BacklashSequenceBuilder : public VMF::SequenceBuilderBase
		{
		protected:
            VMF::VatSequencePtr BuildSequence(std::string sequenceName) override
			{
                VMF::VatSequencePtr seq(new VMF::VatSequence(sequenceName));

				seq->AddTask(VMF::TaskStepPtr(new CLoad1PerformBacklashScanningTask()));

				return VMF::VatSequencePtr(seq.release());
			}
		};
	}
}
