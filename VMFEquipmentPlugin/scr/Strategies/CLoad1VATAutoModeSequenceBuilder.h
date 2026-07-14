#pragma once
#include "SequenceBuilderBase.h"
#include "Tasks\CLoad1VATMoveToStartPositionTask.h"
#include "Tasks\CLoad1VATPerformCalibrationTask.h"
#include "Tasks\CLoad1VATPerformHandPitchScanningTask.h"
#include "Tasks\CLoad1VATFindAlignPosTask.h"
#include "Tasks\CLoad1VATCommitTeachingPosTask.h"

namespace VMF_PLUGIN
{
	// ----------------------------------------------------------------
	// CLoad1VATAutoModeSequenceBuilder
	//
	// 3개 그룹의 Task를 하나의 Sequence로 구성.
	// 각 Task는 SetTaskParams()로 면별 파라미터를 직접 주입.
	// ----------------------------------------------------------------
	class CLoad1VATAutoModeSequenceBuilder : public VMF::SequenceBuilderBase
	{
	public:
		std::string GetSequenceName() const { return "Load1AutoMode"; }

	protected:
		VMF::SequencePtr BuildSequence(const std::string& sequenceName) override
		{
			VMF::SequencePtr seq(new VMF::Sequence(sequenceName));

			// ── HandPitch: 하단카메라 (CameraIndex=1) ──────────────
			{
				auto task = std::make_shared<CLoad1VATMoveToStartPositionTask>();
				VMF::VisionParams mp;
				mp.visionParams["CameraIndex"] = "1";
				mp.visionParams["InspectionType"] = "1";
				mp.visionParams["nMovePart"] = "0";
				mp.visionParams["maxInspCount"] = "1";
				mp.visionParams["bSaveImage"] = "0";
				mp.visionParams["nFovDirection"] = "0";
				mp.visionParams["TIMEOUT_MS"] = "7000";
				task->SetTaskParams(mp);
				seq->AddTask(task);
			}
			{
				auto task = std::make_shared<CLoad1VATPerformCalibrationTask>();
				VMF::VisionParams ip;
				ip.visionParams["CameraIndex"] = "1";
				ip.visionParams["InspectionType"] = "1";
				ip.visionParams["nMovePart"] = "0";
				ip.visionParams["maxInspCount"] = "1";
				ip.visionParams["bSaveImage"] = "0";
				ip.visionParams["nFovDirection"] = "0";
				task->SetTaskParams(ip);
				seq->AddTask(task);
			}
			{
				auto task = std::make_shared<CLoad1VATPerformHandPitchScanningTask>();
				VMF::VisionParams ip;
				ip.visionParams["CameraIndex"] = "1";
				ip.visionParams["InspectionType"] = "1";
				ip.visionParams["nMovePart"] = "0";
				ip.visionParams["maxInspCount"] = "1";
				ip.visionParams["bSaveImage"] = "0";
				ip.visionParams["nFovDirection"] = "0";
				task->SetTaskParams(ip);
				seq->AddTask(task);
			}

			// ── LeftCam: 상단 좌 카메라 (CameraIndex=6) ──────────
			{
				auto task = std::make_shared<CLoad1VATMoveToStartPositionTask>();
				VMF::VisionParams mp;
				mp.visionParams["CameraIndex"] = "6";
				mp.visionParams["InspectionType"] = "5";
				mp.visionParams["nMovePart"] = "2";
				mp.visionParams["maxInspCount"] = "3";
				mp.visionParams["bSaveImage"] = "0";
				mp.visionParams["nFovDirection"] = "0";
				mp.visionParams["TIMEOUT_MS"] = "7000";
				task->SetTaskParams(mp);
				seq->AddTask(task);
			}
			{
				auto task = std::make_shared<CLoad1VATFindAlignPosTask>();
				VMF::VisionParams ip;
				ip.visionParams["CameraIndex"] = "6";
				ip.visionParams["InspectionType"] = "5";
				ip.visionParams["nMovePart"] = "2";
				ip.visionParams["maxInspCount"] = "3";
				ip.visionParams["bSaveImage"] = "0";
				ip.visionParams["nFovDirection"] = "0";
				task->SetTaskParams(ip);
				seq->AddTask(task);
			}
			{
				auto task = std::make_shared<CLoad1VATCommitTeachingPosTask>();
				VMF::VisionParams ip;
				ip.visionParams["CameraIndex"] = "6";
				ip.visionParams["InspectionType"] = "5";
				ip.visionParams["nMovePart"] = "2";
				ip.visionParams["maxInspCount"] = "3";
				ip.visionParams["bSaveImage"] = "0";
				ip.visionParams["nFovDirection"] = "0";
				task->SetTaskParams(ip);
				seq->AddTask(task);
			}

			// ── RightCam: 상단 우 카메라 (CameraIndex=7) ─────────
			{
				auto task = std::make_shared<CLoad1VATMoveToStartPositionTask>();
				VMF::VisionParams mp;
				mp.visionParams["CameraIndex"] = "7";
				mp.visionParams["InspectionType"] = "5";
				mp.visionParams["nMovePart"] = "1";
				mp.visionParams["maxInspCount"] = "3";
				mp.visionParams["bSaveImage"] = "0";
				mp.visionParams["nFovDirection"] = "0";
				mp.visionParams["VisionFOVRequestId"] = "7";
				mp.visionParams["TIMEOUT_MS"] = "7000";
				task->SetTaskParams(mp);
				seq->AddTask(task);
			}
			{
				auto task = std::make_shared<CLoad1VATFindAlignPosTask>();
				VMF::VisionParams ip;
				ip.visionParams["CameraIndex"] = "7";
				ip.visionParams["InspectionType"] = "5";
				ip.visionParams["nMovePart"] = "1";
				ip.visionParams["maxInspCount"] = "3";
				ip.visionParams["bSaveImage"] = "0";
				ip.visionParams["nFovDirection"] = "0";
				ip.visionParams["VisionFOVRequestId"] = "7";
				task->SetTaskParams(ip);
				seq->AddTask(task);
			}
			{
				auto task = std::make_shared<CLoad1VATCommitTeachingPosTask>();
				VMF::VisionParams ip;
				ip.visionParams["CameraIndex"] = "7";
				ip.visionParams["InspectionType"] = "5";
				ip.visionParams["nMovePart"] = "1";
				ip.visionParams["maxInspCount"] = "3";
				ip.visionParams["bSaveImage"] = "0";
				ip.visionParams["nFovDirection"] = "0";
				ip.visionParams["VisionFOVRequestId"] = "7";
				task->SetTaskParams(ip);
				seq->AddTask(task);
			}

			return VMF::SequencePtr(seq.release());
		}
	};
} // namespace VMF_PLUGIN
