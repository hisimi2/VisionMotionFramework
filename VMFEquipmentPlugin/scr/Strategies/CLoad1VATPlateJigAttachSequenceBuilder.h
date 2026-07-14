#pragma once
#include "SequenceBuilderBase.h"
#include "Tasks\CLoad1VATMoveToStartPositionTask.h"
#include "Tasks\CLoad1VATPerformFocusScanningTask.h"
#include "Tasks\CLoad1VATPerformCalibrationTask.h"
#include "Tasks\CLoad1VATPerformFOVScanningTask.h"
#include "Tasks\CLoad1VATFindAlignPosTask.h"

namespace VAT_LOAD1
{
	namespace Sequence
	{
		using namespace VAT_LOAD1::Task;

		// ----------------------------------------------------------------
		// CLoad1VATPlateJigAttachSequenceBuilder
		//
		// PlateJig 장착 시퀀스:
		//   [ZFocus_L]   Cam6 ZFocus 검사
		//   [ZFocus_R]   Cam7 ZFocus 검사
		//   [JigFOV_L]   Cam6 PlateJig FOV 검사
		//   [JigFOV_R]   Cam7 PlateJig FOV 검사
		//   [JigCheck_L] Cam6 PlateJig Check (TargetA/B/Upper)
		//   [JigCheck_R] Cam7 PlateJig Check (TargetA/B/Upper)
		// ----------------------------------------------------------------
		class CLoad1VATPlateJigAttachSequenceBuilder : public VMF::SequenceBuilderBase
		{
		public:
			std::string GetSequenceName() const { return "Load1PlateJigAttach"; }

		protected:
			VMF::SequencePtr BuildSequence(const std::string& sequenceName) override
			{
				VMF::SequencePtr seq(new VMF::Sequence(sequenceName));

				// ── ZFocus 검사 (Cam6) ────────────────────────────────
				seq->AddTask(MakeTaggedTask<CLoad1VATMoveToStartPositionTask>("ZFocus_L"));
				seq->AddTask(MakeTaggedTask<CLoad1VATPerformFocusScanningTask>("ZFocus_L"));

				// ── ZFocus 검사 (Cam7) ────────────────────────────────
				seq->AddTask(MakeTaggedTask<CLoad1VATMoveToStartPositionTask>("ZFocus_R"));
				seq->AddTask(MakeTaggedTask<CLoad1VATPerformFocusScanningTask>("ZFocus_R"));

				// ── PlateJig FOV 검사 (Cam6) ──────────────────────────
				seq->AddTask(MakeTaggedTask<CLoad1VATMoveToStartPositionTask>("JigFOV_L"));
				seq->AddTask(MakeTaggedTask<CLoad1VATPerformCalibrationTask>("JigFOV_L"));
				seq->AddTask(MakeTaggedTask<CLoad1VATPerformFOVScanningTask>("JigFOV_L"));

				// ── PlateJig FOV 검사 (Cam7) ──────────────────────────
				seq->AddTask(MakeTaggedTask<CLoad1VATMoveToStartPositionTask>("JigFOV_R"));
				seq->AddTask(MakeTaggedTask<CLoad1VATPerformCalibrationTask>("JigFOV_R"));
				seq->AddTask(MakeTaggedTask<CLoad1VATPerformFOVScanningTask>("JigFOV_R"));

				// ── PlateJig Check (Cam6) ─────────────────────────────
				seq->AddTask(MakeTaggedTask<CLoad1VATMoveToStartPositionTask>("JigCheck_L"));
				seq->AddTask(MakeTaggedTask<CLoad1VATFindAlignPosTask>("JigCheck_L"));

				// ── PlateJig Check (Cam7) ─────────────────────────────
				seq->AddTask(MakeTaggedTask<CLoad1VATMoveToStartPositionTask>("JigCheck_R"));
				seq->AddTask(MakeTaggedTask<CLoad1VATFindAlignPosTask>("JigCheck_R"));

				return VMF::SequencePtr(seq.release());
			}
		};

	} // namespace Sequence
} // namespace VAT_LOAD1