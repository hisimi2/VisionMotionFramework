#pragma once
#include "SequenceBuilderBase.h"
#include "..\DefineVAT.h"

#include "Tasks/SampleMoveToStartPositionTask.h"
#include "Tasks/SamplePerformFocusScanningTask.h"

namespace VMF_Sample
{
	namespace Sequence
	{
		using namespace VMF_Sample::Task;

		class SampleZFocusSequenceBuilder : public VMF::SequenceBuilderBase
		{
		protected:
			VMF::SequencePtr BuildSequence(const std::string& sequenceName) override
			{
				VMF::SequencePtr seq(new VMF::Sequence(sequenceName));
				seq->AddTask(std::make_shared<SampleMoveToStartPositionTask>());
				seq->AddTask(std::make_shared<SamplePerformFocusScanningTask>());
				return VMF::SequencePtr(seq.release());
			}
		};
	} // namespace Sequence
} // namespace VMF_Sample
