#pragma once
#include "SequenceBuilderBase.h"
#include "Tasks\CLoad1VATMoveToStartPositionTask.h"
#include "Tasks\CLoad1VATPerformCalibrationTask.h"
#include "Tasks\CLoad1VATPerformPickerFOVScanningTask.h"
#include "Tasks\CLoad1VATCommitPickerCamDistanceResultsTask.h"
#include "Tasks\CLoad1VATPerformFOVScanningTask.h"

namespace VAT_LOAD1
{
	namespace Sequence
	{
		using namespace VAT_LOAD1::Task;

		// ----------------------------------------------------------------
		// CLoad1VATPlateJigDetachSequenceBuilder
		//
		// PlateJig 탈착 시퀀스:
		//   [LowCamFOV]  하단카메라 FOV 검사 (Cam1, Picker FOV)
		//   [LowCam]     하단카메라 기준열 검사 (Cam1, Picker 위치)
		//   [UpperFOV_L] 상단카메라 FOV 검사 (Cam6, LoadTable1/2)
		//   [UpperFOV_R] 상단카메라 FOV 검사 (Cam7, CTray1/2)
		// ----------------------------------------------------------------
		class CLoad1VATPlateJigDetachSequenceBuilder : public VMF::SequenceBuilderBase
		{
		public:
			std::string GetSequenceName() const { return "Load1PlateJigDetach"; }

		protected:
			VMF::SequencePtr BuildSequence(const std::string& sequenceName) override
			{
				VMF::SequencePtr seq(new VMF::Sequence(sequenceName));

				// ── 하단카메라 FOV 검사 (Cam1) ───────────────────────
				seq->AddTask(MakeTaggedTask<CLoad1VATMoveToStartPositionTask>("LowCamFOV"));
				seq->AddTask(MakeTaggedTask<CLoad1VATPerformCalibrationTask>("LowCamFOV"));
				seq->AddTask(MakeTaggedTask<CLoad1VATPerformPickerFOVScanningTask>("LowCamFOV"));

				// ── 하단카메라 기준열 검사 (Cam1) ────────────────────
				seq->AddTask(MakeTaggedTask<CLoad1VATMoveToStartPositionTask>("LowCam"));
				seq->AddTask(MakeTaggedTask<CLoad1VATPerformCalibrationTask>("LowCam"));
				seq->AddTask(MakeTaggedTask<CLoad1VATCommitPickerCamDistanceResultsTask>("LowCam"));

				// ── 상단카메라 FOV 검사 (Cam6, LoadTable1/2) ─────────
				seq->AddTask(MakeTaggedTask<CLoad1VATMoveToStartPositionTask>("UpperFOV_L"));
				seq->AddTask(MakeTaggedTask<CLoad1VATPerformCalibrationTask>("UpperFOV_L"));
				seq->AddTask(MakeTaggedTask<CLoad1VATPerformFOVScanningTask>("UpperFOV_L"));

				// ── 상단카메라 FOV 검사 (Cam7, CTray1/2) ─────────────
				seq->AddTask(MakeTaggedTask<CLoad1VATMoveToStartPositionTask>("UpperFOV_R"));
				seq->AddTask(MakeTaggedTask<CLoad1VATPerformCalibrationTask>("UpperFOV_R"));
				seq->AddTask(MakeTaggedTask<CLoad1VATPerformFOVScanningTask>("UpperFOV_R"));

				return VMF::SequencePtr(seq.release());
			}
		};

	} // namespace Sequence
} // namespace VAT_LOAD1