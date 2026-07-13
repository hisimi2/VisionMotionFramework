#pragma once
#include "SequenceBuilderBase.h"
#include "..\VMFEquipmentPlugin/Tasks/CSetPlate1PLVIMoveToScanStartTask.h"
#include "..\VMFEquipmentPlugin/Tasks/CSetPlate1PLVIPerformScanTask.h"

namespace VMF_PLVI
{
	namespace Sequence
	{
		using namespace VMF_PLVI::Task;

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

	} // namespace Sequence
} // namespace VMF_PLVI