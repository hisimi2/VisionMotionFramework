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
		class CLoad1LowCamCheckSequenceBuilder : public DVH_VAT::SequenceBuilderBase
		{
		protected:
            DVH_VAT::VatSequencePtr BuildSequence(std::string sequenceName) override
			{
                DVH_VAT::VatSequencePtr seq(new DVH_VAT::VatSequence(sequenceName));
				seq->AddTask(DVH_VAT::TaskStepPtr(new CLoad1MoveToStartPositionTask()));
				seq->AddTask(DVH_VAT::TaskStepPtr(new CLoad1PerformCalibrationTask()));
				seq->AddTask(DVH_VAT::TaskStepPtr(new CLoad1CommitPickerCamDistanceResultsTask()));
				return DVH_VAT::VatSequencePtr(seq.release());
			}
		};
	}
}
