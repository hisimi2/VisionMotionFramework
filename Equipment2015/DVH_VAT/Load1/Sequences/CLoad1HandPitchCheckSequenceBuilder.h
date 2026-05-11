#pragma once

#include "SequenceBuilderBase.h"
#include "CLoad1MoveToStartPositionTask.h"
#include "CLoad1PerformCalibrationTask.h"
#include "CLoad1PerformHandPitchScanningTask.h"

namespace VAT_LOAD1
{
	namespace Sequence
	{
        using namespace VAT_LOAD1::Task;

		class CLoad1HandPitchCheckSequenceBuilder : public VMF::SequenceBuilderBase
		{
		protected:
            VMF::VatSequencePtr BuildSequence(std::string sequenceName) override
			{
                VMF::VatSequencePtr seq(new VMF::VatSequence(sequenceName));

				seq->AddTask(VMF::TaskStepPtr(new CLoad1MoveToStartPositionTask()));
				seq->AddTask(VMF::TaskStepPtr(new CLoad1PerformCalibrationTask()));
				seq->AddTask(VMF::TaskStepPtr(new CLoad1PerformHandPitchScanningTask()));

				return VMF::VatSequencePtr(seq.release());
			}
		};
	}
}
