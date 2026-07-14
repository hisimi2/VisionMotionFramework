#pragma once
#include "SequenceBuilderBase.h"
#include "Tasks\CLoad1VATMoveToStartPositionTask.h"
#include "Tasks\CLoad1VATPerformCalibrationTask.h"
#include "Tasks\CLoad1VATPerformHandPitchScanningTask.h"
#include "Tasks\CLoad1VATFindAlignPosTask.h"
#include "Tasks\CLoad1VATCommitTeachingPosTask.h"

namespace VAT_LOAD1
{
	namespace Sequence
	{
		using namespace VAT_LOAD1::Task;

		// ----------------------------------------------------------------
		// CLoad1VATAutoModeSequenceBuilder
		//
		// 3개 그룹의 Task를 하나의 Sequence로 구성.
		// 각 Task에 태그를 지정 → 실행 전 ctx.ApplyTagParams(tag) 자동 호출.
		//
		// 태그 의미 (Strategy::ConfigureParams에서 동일 태그로 등록):
		//   "HandPitch" - 하단카메라, 기준 Picker 위치 측정
		//   "LeftCam"   - Cam6, LoadTable1/2 + CTray1
		//   "RightCam"  - Cam7, CTray2
		// ----------------------------------------------------------------
		class CLoad1VATAutoModeSequenceBuilder : public VMF::SequenceBuilderBase
		{
		public:
			std::string GetSequenceName() const { return "Load1AutoMode"; }

		protected:
			VMF::SequencePtr BuildSequence(const std::string& sequenceName) override
			{
				VMF::SequencePtr seq(new VMF::Sequence(sequenceName));

				// ── 그룹 1: HandPitch 검사 ────────────────────────────
				seq->AddTask(MakeTaggedTask<CLoad1VATMoveToStartPositionTask>("HandPitch"));
				seq->AddTask(MakeTaggedTask<CLoad1VATPerformCalibrationTask>("HandPitch"));
				seq->AddTask(MakeTaggedTask<CLoad1VATPerformHandPitchScanningTask>("HandPitch"));

				// ── 그룹 2: Left 상단카메라 Teaching (Cam6) ──────────
				seq->AddTask(MakeTaggedTask<CLoad1VATMoveToStartPositionTask>("LeftCam"));
				seq->AddTask(MakeTaggedTask<CLoad1VATFindAlignPosTask>("LeftCam"));
				seq->AddTask(MakeTaggedTask<CLoad1VATCommitTeachingPosTask>("LeftCam"));

				// ── 그룹 3: Right 상단카메라 Teaching (Cam7) ─────────
				seq->AddTask(MakeTaggedTask<CLoad1VATMoveToStartPositionTask>("RightCam"));
				seq->AddTask(MakeTaggedTask<CLoad1VATFindAlignPosTask>("RightCam"));
				seq->AddTask(MakeTaggedTask<CLoad1VATCommitTeachingPosTask>("RightCam"));

				return VMF::SequencePtr(seq.release());
			}
		};

	} // namespace Sequence
} // namespace VAT_LOAD1