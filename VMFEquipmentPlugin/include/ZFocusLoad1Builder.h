#pragma once
#include "SequenceBuilderBase.h"
#include "DefineVAT.h"

#include "TaskLoad1MoveToStartPosition.h"
#include "TaskLoad1PerformFocusScanning.h"

namespace VMF_Sample
{
	/// <summary>
	/// VisionParams에 VisionPosition을 추가하는 헬퍼 함수 (3축)
	/// </summary>
	inline void AddVisionPoint(VMF::VisionParams& params, int locateId, int requestId,
		double x, double y, double z)
	{
		std::vector<double> pos;
		pos.push_back(x);
		pos.push_back(y);
		pos.push_back(z);
		params.visionPositions.push_back(VMF::VisionPosition(pos, locateId, requestId));
	}

	/// <summary>
	/// VisionParams에 VisionPosition을 추가하는 헬퍼 함수 (5축)
	/// </summary>
	inline void AddVisionPoint(VMF::VisionParams& params, int locateId, int requestId,
		double x, double y, double z, double t1, double t2)
	{
		std::vector<double> pos;
		pos.push_back(x);
		pos.push_back(y);
		pos.push_back(z);
		pos.push_back(t1);
		pos.push_back(t2);
		params.visionPositions.push_back(VMF::VisionPosition(pos, locateId, requestId));
	}

	class ZFocusLoad1Builder : public VMF::SequenceBuilderBase
	{
	protected:
		VMF::SequencePtr BuildSequence(const std::string& sequenceName) override
		{
			VMF::SequencePtr seq(new VMF::Sequence(sequenceName));

			// [1] Task_MoveToStartPosition 생성 및 params 주입
			{
				auto task = std::make_shared<TaskLoad1MoveToStartPosition>();

				VMF::VisionParams moveParams;
				moveParams.visionParams[VAT_SEQ_PARAM_TIMEOUT_MS] = "10000";
				AddVisionPoint(moveParams, 1, 1, 125.3, 48.7, -2.5);
				AddVisionPoint(moveParams, 2, 2, 130.1, 52.3, -1.8);
				AddVisionPoint(moveParams, 3, 3, 118.9, 45.6, -3.2);

				task->SetTaskParams(moveParams);
				seq->AddTask(task);
			}

			// [2] Task_PerformFocusScanning 생성 및 params 주입
			{
				auto task = std::make_shared<TaskLoad1PerformFocusScanning>();

				VMF::VisionParams focusParams;
				const int cameraIndex   = 6;
				const int packageId     = 1;
				const int pickerMaxRow  = 4;
				const int pickerMaxCol  = 9;

				focusParams.visionParams[VAT_SEQ_PARAM_CAMERA_INDEX]        = std::to_string(cameraIndex);
				focusParams.visionParams[VAT_SEQ_PARAM_PACKAGE_ID]          = std::to_string(packageId);
				focusParams.visionParams[VAT_SEQ_PARAM_MOTION_TIMEOUT_MS]   = "7000";
				focusParams.visionParams[VAT_SEQ_PARAM_VISION_TIMEOUT_MS]   = "30000";

				AddVisionPoint(focusParams, 1, 1, 125.3, 48.7, -2.5);
				AddVisionPoint(focusParams, 2, 2, 130.1, 52.3, -1.8);
				AddVisionPoint(focusParams, 3, 3, 118.9, 45.6, -3.2);

				if (cameraIndex > 5)
				{
					AddVisionPoint(focusParams, 3, 3, 118.9, 45.6, -3.2);
					AddVisionPoint(focusParams, 4, 4, 122.4, 50.1, -2.1);
					AddVisionPoint(focusParams, 5, 5, 128.7, 47.9, -2.9);
				}

				for (int row = 0; row < pickerMaxRow; ++row)
				{
					for (int col = 0; col < pickerMaxCol; ++col)
					{
						double pickerX = 200.0 + col * 25.0;
						double pickerY = 100.0 + row * 30.0;
						double pickerZ = -5.0;
						int locateId = 20 + row * 10 + col;
						AddVisionPoint(focusParams, locateId, locateId, pickerX, pickerY, pickerZ);
					}
				}

				task->SetTaskParams(focusParams);
				seq->AddTask(task);
			}

			return VMF::SequencePtr(seq.release());
		}
	};
} // namespace VMF_Sample
