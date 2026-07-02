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

		/// <summary>
		/// [Sample] Z Focus 검사 시퀀스 빌더
		/// 
		/// [책임 범위 - 오직 Task 조립만!]
		/// - Task 생성
		/// - Task 순서 결정 (MoveToStartPosition → PerformFocusScanning)
		/// - seq->AddTask()로 시퀀스에 추가
		/// 
		/// ※ SetTaskParams()는 호출하지 않음!
		///   모든 파라미터(전역 + Task별)는 Strategy.ConfigureParams()에서 결정
		/// 
		/// [Strategy와의 책임 분리]
		/// - SampleSequenceStrategy.ConfigureParams(): 
		///     Context 전역 params (3순위) + Task별 params (2순위) 설정
		/// - SampleZFocusSequenceBuilder.BuildSequence(): 
		///     오직 Task 조립만! (params 몰라도 됨)
		/// 
		/// [데이터 우선순위]
		///   Task.GetTaskSeqParamAs(ctx, key, default)
		///     → 1순위: Task 자체 params (m_taskParams_ — Builder가 설정 안 함)
		///     → 2순위: Context의 Task별 params (Strategy.ConfigureParams에서 ctx.SetTaskParams)
		///     → 3순위: Context 전역 params (Strategy.ConfigureParams에서 ctx.SetVisionParams)
		/// </summary>
		class SampleZFocusSequenceBuilder : public VMF::SequenceBuilderBase
		{
		protected:
			VMF::SequencePtr BuildSequence(const std::string& sequenceName) override
			{
				VMF::SequencePtr seq(new VMF::Sequence(sequenceName));

				// ──────────────────────────────────────────────
				// Task 1: MoveToStartPosition
				// ──────────────────────────────────────────────
				// 파라미터는 Strategy.ConfigureParams()에서 설정 완료.
				// Builder는 Task 생성 및 추가만!
				seq->AddTask(std::make_shared<SampleMoveToStartPositionTask>());

				// ──────────────────────────────────────────────
				// Task 2: PerformFocusScanning
				// ──────────────────────────────────────────────
				// cameraIndex, packageId, timeouts 등 모든 파라미터는
				// Strategy가 Context 전역 params + Task params로 설정 완료.
				seq->AddTask(std::make_shared<SamplePerformFocusScanningTask>());

				return VMF::SequencePtr(seq.release());
			}

		};
	} // namespace Sequence
} // namespace VMF_Sample
