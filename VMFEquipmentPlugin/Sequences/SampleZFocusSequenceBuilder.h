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

				// [1] Task_MoveToStartPosition 생성 및 params 주입
				{
					auto task = std::make_shared<SampleMoveToStartPositionTask>();
					auto it = m_taskParamsMap.find(task->GetName());
					if (it != m_taskParamsMap.end())
						task->SetTaskParams(it->second);
					seq->AddTask(task);
				}

				// [2] Task_PerformFocusScanning 생성 및 params 주입
				{
					auto task = std::make_shared<SamplePerformFocusScanningTask>();
					auto it = m_taskParamsMap.find(task->GetName());
					if (it != m_taskParamsMap.end())
						task->SetTaskParams(it->second);
					seq->AddTask(task);
				}

				return VMF::SequencePtr(seq.release());
			}
		};
	} // namespace Sequence
} // namespace VMF_Sample
