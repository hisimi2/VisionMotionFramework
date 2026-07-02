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
		/// [책임 범위]
		/// - Task 생성 및 순서 조립 (MoveToStartPosition → PerformFocusScanning)
		/// - Task별로 Strategy의 전역 파라미터와 다른 값이 필요한 경우에만
		///   SetTaskParams()로 override (대부분의 파라미터는 Strategy가 Context에 설정한
		///   전역 파라미터를 fallback하여 사용하므로 Builder에서 중복 설정 불필요)
		/// 
		/// [Strategy와의 책임 분리]
		/// - SampleSequenceStrategy.ConfigureParams(): 
		///     Context 전역 params 설정 (cameraIndex, packageId, timeouts, visionPositions)
		/// - SampleZFocusSequenceBuilder.BuildSequence(): 
		///     Task 조립 + Task별 override만 설정
		/// 
		/// [데이터 흐름: Builder → Task → Context]
		///   Builder: SetTaskParams(taskParams)  ──▶  Task.m_taskParams_ (1순위)
		///   Strategy: ctx.SetVisionParams(global) ──▶  Context 전역 params     (3순위 - fallback)
		///   Task 수행 시: GetTaskSeqParamAs(ctx, key, default) 
		///     → 1순위: Task 자체 params (Builder가 주입)
		///     → 2순위: Context의 Task별 params (ctx.SetTaskParams)
		///     → 3순위: Context 전역 params (Strategy가 설정)
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
				// Strategy.ConfigureParams()에서 Context 전역 params로
				// VAT_SEQ_PARAM_MOTION_TIMEOUT_MS (7000ms)가 설정되어 있음.
				// Task는 VAT_SEQ_PARAM_TIMEOUT_MS ("TimeOutMs") 키로 읽으므로,
				// Builder에서 override 시 동일 키를 사용해야 함.
				{
					auto moveTask = std::make_shared<SampleMoveToStartPositionTask>();
					
					// [Override 예시] Move task만 더 긴 timeout (10초) 필요
					VMF::VisionParams taskParams;
					taskParams.seqParams[VAT_SEQ_PARAM_TIMEOUT_MS] = "10000";
					moveTask->SetTaskParams(taskParams);

					seq->AddTask(moveTask);
				}

				// ──────────────────────────────────────────────
				// Task 2: PerformFocusScanning
				// ──────────────────────────────────────────────
				// cameraIndex, packageId, motion_timeout_ms 등 대부분의 파라미터는
				// Strategy.ConfigureParams()에서 Context 전역 params로 설정되어 있으므로
				// Builder에서 중복 설정할 필요 없음.
				// Task가 GetTaskSeqParamAs()로 조회하면 자동으로 전역 params로 fallback됨.
				//
				// ※ vision_timeout_ms만 Focus task에서 더 짧게 override
				{
					auto focusTask = std::make_shared<SamplePerformFocusScanningTask>();
					
					// [Override 예시] Focus task의 vision timeout만 전역(60000ms)과 다르게 설정
					VMF::VisionParams taskParams;
					taskParams.seqParams[VAT_SEQ_PARAM_VISION_TIMEOUT_MS] = "30000";
					focusTask->SetTaskParams(taskParams);

					seq->AddTask(focusTask);
				}

				return VMF::SequencePtr(seq.release());
			}

		};
	} // namespace Sequence
} // namespace VMF_Sample
