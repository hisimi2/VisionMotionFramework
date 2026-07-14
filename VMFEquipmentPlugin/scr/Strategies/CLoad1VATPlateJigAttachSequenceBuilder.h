#pragma once
#include "SequenceBuilderBase.h"
#include "Tasks\CLoad1VATMoveToStartPositionTask.h"
#include "Tasks\CLoad1VATPerformFocusScanningTask.h"
#include "Tasks\CLoad1VATPerformCalibrationTask.h"
#include "Tasks\CLoad1VATPerformFOVScanningTask.h"
#include "Tasks\CLoad1VATFindAlignPosTask.h"

namespace VMF_PLUGIN
{
	namespace Sequence
	{
		using namespace VMF_PLUGIN::Task;

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
				{
					auto task = std::make_shared<CLoad1VATMoveToStartPositionTask>();
					VMF::VisionParams mp;
					mp.visionParams["CameraIndex"] = "6";
					mp.visionParams["CameraID"] = "3128";
					mp.visionParams["InspectionType"] = "6";
					mp.visionParams["nMovePart"] = "0";
					mp.visionParams["TIMEOUT_MS"] = "7000";
					task->SetTaskParams(mp);
					seq->AddTask(task);
				}
				{
					auto task = std::make_shared<CLoad1VATPerformFocusScanningTask>();
					VMF::VisionParams ip;
					ip.visionParams["CameraIndex"] = "6";
					ip.visionParams["CameraID"] = "3128";
					ip.visionParams["InspectionType"] = "6";
					ip.visionParams["nMovePart"] = "0";
					task->SetTaskParams(ip);
					seq->AddTask(task);
				}

				// ── ZFocus 검사 (Cam7) ────────────────────────────────
				{
					auto task = std::make_shared<CLoad1VATMoveToStartPositionTask>();
					VMF::VisionParams mp;
					mp.visionParams["CameraIndex"] = "7";
					mp.visionParams["CameraID"] = "3130";
					mp.visionParams["InspectionType"] = "6";
					mp.visionParams["nMovePart"] = "0";
					mp.visionParams["TIMEOUT_MS"] = "7000";
					task->SetTaskParams(mp);
					seq->AddTask(task);
				}
				{
					auto task = std::make_shared<CLoad1VATPerformFocusScanningTask>();
					VMF::VisionParams ip;
					ip.visionParams["CameraIndex"] = "7";
					ip.visionParams["CameraID"] = "3130";
					ip.visionParams["InspectionType"] = "6";
					ip.visionParams["nMovePart"] = "0";
					task->SetTaskParams(ip);
					seq->AddTask(task);
				}

				// ── PlateJig FOV 검사 (Cam6) ──────────────────────────
				{
					auto task = std::make_shared<CLoad1VATMoveToStartPositionTask>();
					VMF::VisionParams mp;
					mp.visionParams["CameraIndex"] = "6";
					mp.visionParams["CameraID"] = "3128";
					mp.visionParams["InspectionType"] = "9";
					mp.visionParams["nMovePart"] = "0";
					mp.visionParams["maxInspCount"] = "1";
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
					ip.visionParams["maxInspCount"] = "1";
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
					ip.visionParams["maxInspCount"] = "1";
					ip.visionParams["VisionFOVRequestId"] = "7";
					task->SetTaskParams(ip);
					seq->AddTask(task);
				}

				// ── PlateJig FOV 검사 (Cam7) ──────────────────────────
				{
					auto task = std::make_shared<CLoad1VATMoveToStartPositionTask>();
					VMF::VisionParams mp;
					mp.visionParams["CameraIndex"] = "7";
					mp.visionParams["CameraID"] = "3130";
					mp.visionParams["InspectionType"] = "9";
					mp.visionParams["nMovePart"] = "0";
					mp.visionParams["maxInspCount"] = "1";
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
					ip.visionParams["maxInspCount"] = "1";
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
					ip.visionParams["maxInspCount"] = "1";
					ip.visionParams["VisionFOVRequestId"] = "7";
					task->SetTaskParams(ip);
					seq->AddTask(task);
				}

				// ── PlateJig Check (Cam6) ─────────────────────────────
				{
					auto task = std::make_shared<CLoad1VATMoveToStartPositionTask>();
					VMF::VisionParams mp;
					mp.visionParams["CameraIndex"] = "6";
					mp.visionParams["CameraID"] = "3128";
					mp.visionParams["InspectionType"] = "5";
					mp.visionParams["nMovePart"] = "5";
					mp.visionParams["maxInspCount"] = "3";
					mp.visionParams["TIMEOUT_MS"] = "7000";
					task->SetTaskParams(mp);
					seq->AddTask(task);
				}
				{
					auto task = std::make_shared<CLoad1VATFindAlignPosTask>();
					VMF::VisionParams ip;
					ip.visionParams["CameraIndex"] = "6";
					ip.visionParams["CameraID"] = "3128";
					ip.visionParams["InspectionType"] = "5";
					ip.visionParams["nMovePart"] = "5";
					ip.visionParams["maxInspCount"] = "3";
					task->SetTaskParams(ip);
					seq->AddTask(task);
				}

				// ── PlateJig Check (Cam7) ─────────────────────────────
				{
					auto task = std::make_shared<CLoad1VATMoveToStartPositionTask>();
					VMF::VisionParams mp;
					mp.visionParams["CameraIndex"] = "7";
					mp.visionParams["CameraID"] = "3130";
					mp.visionParams["InspectionType"] = "5";
					mp.visionParams["nMovePart"] = "5";
					mp.visionParams["maxInspCount"] = "3";
					mp.visionParams["TIMEOUT_MS"] = "7000";
					task->SetTaskParams(mp);
					seq->AddTask(task);
				}
				{
					auto task = std::make_shared<CLoad1VATFindAlignPosTask>();
					VMF::VisionParams ip;
					ip.visionParams["CameraIndex"] = "7";
					ip.visionParams["CameraID"] = "3130";
					ip.visionParams["InspectionType"] = "5";
					ip.visionParams["nMovePart"] = "5";
					ip.visionParams["maxInspCount"] = "3";
					task->SetTaskParams(ip);
					seq->AddTask(task);
				}

				return VMF::SequencePtr(seq.release());
			}
		};

	} // namespace Sequence
} // namespace VMF_PLUGIN
