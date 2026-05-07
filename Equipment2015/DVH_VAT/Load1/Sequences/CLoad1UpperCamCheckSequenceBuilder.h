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
		class CLoad1UpperCamCheckSequenceBuilder : public DVH_VAT::SequenceBuilderBase
		{
		protected:
            DVH_VAT::VatSequencePtr BuildSequence(std::string sequenceName) override
			{
                DVH_VAT::VatSequencePtr seq(new DVH_VAT::VatSequence(sequenceName));
				seq->AddTask(DVH_VAT::TaskStepPtr(new CLoad1MoveToStartPositionTask()));
				seq->AddTask(DVH_VAT::TaskStepPtr(new CLoad1FindAlignPosTask()));
				seq->AddTask(DVH_VAT::TaskStepPtr(new CLoad1CommitTeachingPosTask()));
				return DVH_VAT::VatSequencePtr(seq.release());
			}
		};
	}
}
