#pragma once
#include "SequenceBuilderBase.h"

#include "CLoad1MoveToStartPositionTask.h"
#include "CLoad1FindAlignPosTask.h"

namespace VAT_LOAD1
{
	namespace Sequence
	{
        using namespace VAT_LOAD1::Task;
		class CLoad1PlateJigSequenceBuilder : public VMF::SequenceBuilderBase
		{
		protected:
            VMF::VatSequencePtr BuildSequence(std::string sequenceName) override
			{
                VMF::VatSequencePtr seq(new VMF::VatSequence(sequenceName));
				seq->AddTask(VMF::TaskStepPtr(new CLoad1MoveToStartPositionTask()));
				seq->AddTask(VMF::TaskStepPtr(new CLoad1FindAlignPosTask()));
				return VMF::VatSequencePtr(seq.release());
			}
		};
	}
}
