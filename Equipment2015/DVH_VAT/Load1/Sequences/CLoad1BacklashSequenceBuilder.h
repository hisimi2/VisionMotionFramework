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
            VMF::SequencePtr BuildSequence(std::string sequenceName) override
			{
                VMF::SequencePtr seq(new VMF::Sequence(sequenceName));

				seq->AddTask(VMF::TaskStepPtr(new CLoad1PerformBacklashScanningTask()));

				return VMF::SequencePtr(seq.release());
			}
		};
	}
}
