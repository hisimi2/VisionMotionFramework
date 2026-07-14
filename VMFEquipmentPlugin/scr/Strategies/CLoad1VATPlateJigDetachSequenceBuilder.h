#pragma once
#include "SequenceBuilderBase.h"
#include "Tasks\CLoad1VATMoveToStartPositionTask.h"
#include "Tasks\CLoad1VATPerformCalibrationTask.h"
#include "Tasks\CLoad1VATPerformPickerFOVScanningTask.h"
#include "Tasks\CLoad1VATCommitPickerCamDistanceResultsTask.h"
#include "Tasks\CLoad1VATPerformFOVScanningTask.h"

namespace VMF_PLUGIN
{
	namespace Sequence
	{
		using namespace VMF_PLUGIN::Task;

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
				{
					auto task = std::make_shared<CLoad1VATMoveToStartPositionTask>();
					VMF::VisionParams mp;
					mp.visionParams["CameraIndex"] = "1";
					mp.visionParams["maxInspCount"] = "1";
					mp.visionParams["VisionFOVRequestId"] = "7";
					mp.visionParams["TIMEOUT_MS"] = "7000";
					task->SetTaskParams(mp);
					seq->AddTask(task);
				}
				{
					auto task = std::make_shared<CLoad1VATPerformCalibrationTask>();
					VMF::VisionParams ip;
					ip.visionParams["CameraIndex"] = "1";
					ip.visionParams["maxInspCount"] = "1";
					ip.visionParams["VisionFOVRequestId"] = "7";
					task->SetTaskParams(ip);
					seq->AddTask(task);
				}
				{
					auto task = std::make_shared<CLoad1VATPerformPickerFOVScanningTask>();
					VMF::VisionParams ip;
					ip.visionParams["CameraIndex"] = "1";
					ip.visionParams["maxInspCount"] = "1";
					ip.visionParams["VisionFOVRequestId"] = "7";
					task->SetTaskParams(ip);
					seq->AddTask(task);
				}

				// ── 하단카메라 기준열 검사 (Cam1) ────────────────────
				{
					auto task = std::make_shared<CLoad1VATMoveToStartPositionTask>();
					VMF::VisionParams mp;
					mp.visionParams["CameraIndex"] = "1";
					mp.visionParams["CameraID"] = "3028";
					mp.visionParams["InspectionType"] = "1";
					mp.visionParams["nMovePart"] = "0";
					mp.visionParams["maxInspCount"] = "3";
					mp.visionParams["wide_check"] = "1";
					mp.visionParams["TIMEOUT_MS"] = "7000";
					task->SetTaskParams(mp);
					seq->AddTask(task);
				}
				{
					auto task = std::make_shared<CLoad1VATPerformCalibrationTask>();
					VMF::VisionParams ip;
					ip.visionParams["CameraIndex"] = "1";
					ip.visionParams["CameraID"] = "3028";
					ip.visionParams["InspectionType"] = "1";
					ip.visionParams["nMovePart"] = "0";
					ip.visionParams["maxInspCount"] = "3";
					ip.visionParams["wide_check"] = "1";
					task->SetTaskParams(ip);
					seq->AddTask(task);
				}
				{
					auto task = std::make_shared<CLoad1VATCommitPickerCamDistanceResultsTask>();
					VMF::VisionParams ip;
					ip.visionParams["CameraIndex"] = "1";
					ip.visionParams["CameraID"] = "3028";
					ip.visionParams["InspectionType"] = "1";
					ip.visionParams["nMovePart"] = "0";
					ip.visionParams["maxInspCount"] = "3";
					ip.visionParams["wide_check"] = "1";
					task->SetTaskParams(ip);
					seq->AddTask(task);
				}

				// ── 상단카메라 FOV 검사 (Cam6, LoadTable1/2) ─────────
				{
					auto task = std::make_shared<CLoad1VATMoveToStartPositionTask>();
					VMF::VisionParams mp;
					mp.visionParams["CameraIndex"] = "6";
					mp.visionParams["CameraID"] = "3128";
					mp.visionParams["InspectionType"] = "9";
					mp.visionParams["nMovePart"] = "0";
					mp.visionParams["VisionFOVRequestId"] = "7";
					mp.visionParams["TIMEOUT_MS"] = "7000";
					task->SetTaskParams(mp);
					seq->AddTask(task);
				}
				{
					auto task = std::make_shared<CLoad1VATPerformCalibrationTask>();
					VMF::VisionParams ip;
					ip.visionParams["CameraIndex"] = "6";
					ip.visionParams["CameraID"] = "3128";
					ip.visionParams["InspectionType"] = "9";
					ip.visionParams["nMovePart"] = "0";
					ip.visionParams["VisionFOVRequestId"] = "7";
					task->SetTaskParams(ip);
					seq->AddTask(task);
				}
				{
					auto task = std::make_shared<CLoad1VATPerformFOVScanningTask>();
					VMF::VisionParams ip;
					ip.visionParams["CameraIndex"] = "6";
					ip.visionParams["CameraID"] = "3128";
					ip.visionParams["InspectionType"] = "9";
					ip.visionParams["nMovePart"] = "0";
					ip.visionParams["VisionFOVRequestId"] = "7";
					task->SetTaskParams(ip);
					seq->AddTask(task);
				}

				// ── 상단카메라 FOV 검사 (Cam7, CTray1/2) ─────────────
				{
					auto task = std::make_shared<CLoad1VATMoveToStartPositionTask>();
					VMF::VisionParams mp;
					mp.visionParams["CameraIndex"] = "7";
					mp.visionParams["CameraID"] = "3130";
					mp.visionParams["InspectionType"] = "9";
					mp.visionParams["nMovePart"] = "0";
					mp.visionParams["VisionFOVRequestId"] = "7";
					mp.visionParams["TIMEOUT_MS"] = "7000";
					task->SetTaskParams(mp);
					seq->AddTask(task);
				}
				{
					auto task = std::make_shared<CLoad1VATPerformCalibrationTask>();
					VMF::VisionParams ip;
					ip.visionParams["CameraIndex"] = "7";
					ip.visionParams["CameraID"] = "3130";
					ip.visionParams["InspectionType"] = "9";
					ip.visionParams["nMovePart"] = "0";
					ip.visionParams["VisionFOVRequestId"] = "7";
					task->SetTaskParams(ip);
					seq->AddTask(task);
				}
				{
					auto task = std::make_shared<CLoad1VATPerformFOVScanningTask>();
					VMF::VisionParams ip;
					ip.visionParams["CameraIndex"] = "7";
					ip.visionParams["CameraID"] = "3130";
					ip.visionParams["InspectionType"] = "9";
					ip.visionParams["nMovePart"] = "0";
					ip.visionParams["VisionFOVRequestId"] = "7";
					task->SetTaskParams(ip);
					seq->AddTask(task);
				}

				return VMF::SequencePtr(seq.release());
			}
		};

	} // namespace Sequence
} // namespace VMF_PLUGIN