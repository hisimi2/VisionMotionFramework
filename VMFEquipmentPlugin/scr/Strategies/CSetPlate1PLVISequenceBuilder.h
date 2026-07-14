#pragma once
#include "SequenceBuilderBase.h"
#include "scr\Tasks\CSetPlate1PLVIMoveToScanStartTask.h"
#include "scr\Tasks\CSetPlate1PLVIPerformScanTask.h"

namespace VMF_PLUGIN
{
	class CSetPlate1PLVISequenceBuilder : public VMF::SequenceBuilderBase
	{
	protected:
		VMF::SequencePtr BuildSequence(
			const std::string& sequenceName) override
		{
			VMF::SequencePtr seq(new VMF::Sequence(sequenceName));

			seq->AddTask(VMF::TaskPtr(new CSetPlate1PLVIMoveToScanStartTask()));
			seq->AddTask(VMF::TaskPtr(new CSetPlate1PLVIPerformScanTask()));

			return VMF::SequencePtr(seq.release());
		}
	};
} // namespace VMF_PLUGIN
