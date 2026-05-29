#pragma once
#include "SequenceBuilderBase.h"

#include "VMFComposition/Load1/Tasks/CLoad1MoveToStartPositionTask.h"
#include "VMFComposition/Load1/Tasks/CLoad1PerformFocusScanningTask.h"

namespace VMF_Load1
{
	namespace Sequence
	{
        using namespace VMF_Load1::Task;
		class CLoad1ZFocusSequenceBuilder : public VMF::SequenceBuilderBase
		{
		protected:
            VMF::SequencePtr BuildSequence(std::string sequenceName) override
			{
                VMF::SequencePtr seq(new VMF::Sequence(sequenceName));
				seq->AddTask(VMF::TaskPtr(new CLoad1MoveToStartPositionTask()));
				seq->AddTask(VMF::TaskPtr(new CLoad1PerformFocusScanningTask()));
				return VMF::SequencePtr(seq.release());
			}
		};
	}
}
