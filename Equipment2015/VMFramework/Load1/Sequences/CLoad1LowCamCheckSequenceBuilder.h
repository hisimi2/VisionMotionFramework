#pragma once

#include "SequenceBuilderBase.h"
#include "CLoad1MoveToStartPositionTask.h"
#include "CLoad1PerformCalibrationTask.h"
#include "CLoad1CommitPickerCamDistanceResultsTask.h"

namespace VAT_LOAD1
{
	namespace Sequence
	{
        using namespace VAT_LOAD1::Task;
		class CLoad1LowCamCheckSequenceBuilder : public VMF::SequenceBuilderBase
		{
		protected:
            VMF::SequencePtr BuildSequence(std::string sequenceName) override
			{
                VMF::SequencePtr seq(new VMF::Sequence(sequenceName));
				seq->AddTask(VMF::TaskStepPtr(new CLoad1MoveToStartPositionTask()));
				seq->AddTask(VMF::TaskStepPtr(new CLoad1PerformCalibrationTask()));
				seq->AddTask(VMF::TaskStepPtr(new CLoad1CommitPickerCamDistanceResultsTask()));
				return VMF::SequencePtr(seq.release());
			}
		};
	}
}
