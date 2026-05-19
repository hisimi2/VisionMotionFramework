#pragma once
#include "SequenceBuilderBase.h"

#include "CLoad1MoveToStartPositionTask.h"
#include "CLoad1FindAlignPosTask.h"
#include "CLoad1CommitTeachingPosTask.h"

namespace VAT_LOAD1
{
	namespace Sequence
	{
        using namespace VAT_LOAD1::Task;
		class CLoad1UpperCamCheckSequenceBuilder : public VMF::SequenceBuilderBase
		{
		protected:
            VMF::SequencePtr BuildSequence(std::string sequenceName) override
			{
                VMF::SequencePtr seq(new VMF::Sequence(sequenceName));
				seq->AddTask(VMF::TaskStepPtr(new CLoad1MoveToStartPositionTask()));
				seq->AddTask(VMF::TaskStepPtr(new CLoad1FindAlignPosTask()));
				seq->AddTask(VMF::TaskStepPtr(new CLoad1CommitTeachingPosTask()));
				return VMF::SequencePtr(seq.release());
			}
		};
	}
}
