#pragma once
#include "SequenceBuilderBase.h"

#include "CLoad1MoveToStartPositionTask.h"
#include "CLoad1PerformFocusScanningTask.h"

namespace VAT_LOAD1
{
	namespace Sequence
	{
        using namespace VAT_LOAD1::Task;
		class CLoad1ZFocusSequenceBuilder : public VMF::SequenceBuilderBase
		{
		protected:
            VMF::VatSequencePtr BuildSequence(std::string sequenceName) override
			{
                VMF::VatSequencePtr seq(new VMF::VatSequence(sequenceName));
				seq->AddTask(VMF::TaskStepPtr(new CLoad1MoveToStartPositionTask()));
				seq->AddTask(VMF::TaskStepPtr(new CLoad1PerformFocusScanningTask()));
				return VMF::VatSequencePtr(seq.release());
			}
		};
	}
}
