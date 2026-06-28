#pragma once
#include "SequenceBuilderBase.h"

#include "Tasks/SampleMoveToStartPositionTask.h"
#include "Tasks/SamplePerformFocusScanningTask.h"

namespace VMF_Sample
{
	namespace Sequence
	{
		using namespace VMF_Sample::Task;

		/// <summary>
		/// [Sample] Z Focus 검사 시퀀스 빌더
		/// MoveToStartPosition → PerformFocusScanning 순서로 Task를 조립
		/// 
		/// !!! 수정 가이드 !!!
		/// 1. BuildSequence() 내에서 Task 추가 순서는 장비의 공정 순서에 맞게 변경
		/// 2. 장비에 추가 Task가 필요하면 seq->AddTask()로 추가
		///    예: seq->AddTask(TaskPtr(new MyPreAlignTask()));
		/// 3. GetSequenceName() 반환값은 Strategy에서 참조하므로 일관성 유지
		/// </summary>
		class SampleZFocusSequenceBuilder : public VMF::SequenceBuilderBase
		{
		protected:
			VMF::SequencePtr BuildSequence(const std::string& sequenceName) override
			{
				VMF::SequencePtr seq(new VMF::Sequence(sequenceName));
				seq->AddTask(VMF::TaskPtr(new SampleMoveToStartPositionTask()));
				seq->AddTask(VMF::TaskPtr(new SamplePerformFocusScanningTask()));
				return VMF::SequencePtr(seq.release());
			}

		};
	} // namespace Sequence
} // namespace VMF_Sample
