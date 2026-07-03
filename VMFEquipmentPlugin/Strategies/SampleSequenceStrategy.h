#pragma once
#include "DefaultSetupStrategy.h"
#include "Sequences/SampleZFocusSequenceBuilder.h"
#include "VMFEquipmentPluginExport.h"

namespace VMF_Sample
{
	using namespace VMF;
	using namespace Sequence;

    /// <summary>
	/// [Sample] Focus Check Sequence 전략 클래스
	/// DefaultSetupStrategy를 상속받아 Task별 params 설정을 담당
	/// 
	/// [책임 범위]
	/// - CreateRepository(): DB 초기화 (SqliteDataRepository)
	/// - CreateVisionProcessor(): Vision 서버 연결 및 프로세서 초기화
	/// - CreateBuilder(): SampleZFocusSequenceBuilder 반환
	/// - ConfigureParams(): 각 Task별 VisionParams 설정
	///     - CameraIndex, PackageId, 타임아웃 등 Task별 파라미터
	///     - 검사 위치(VisionPosition) 목록 등록
	/// 
	/// [Builder와의 책임 분리]
	/// ╔══════════════════════════════════════════════╗
	/// ║  SampleSequenceStrategy (Strategy)           ║
	/// ║  └─ ConfigureParams(): Task별 params 설정    ║
	/// ║      ├─ Task_MoveToStartPosition:           ║
	/// ║      │   ├─ VAT_SEQ_PARAM_TIMEOUT_MS = 10000║
	/// ║      │   └─ visionPositions (검사 위치 목록) ║
	/// ║      └─ Task_PerformFocusScanning:          ║
	/// ║          ├─ VAT_SEQ_PARAM_CAMERA_INDEX = 6  ║
	/// ║          ├─ VAT_SEQ_PARAM_PACKAGE_ID   = 1  ║
	/// ║          ├─ VAT_SEQ_PARAM_VISION_TIMEOUT = 30000║
	/// ║          └─ visionPositions (검사 위치 목록) ║
	/// ╚══════════════════════════════════════════════╝
	///           │  Context의 Task별 params
	///           ▼
	/// ╔══════════════════════════════════════════════╗
	/// ║  SampleZFocusSequenceBuilder (Builder)       ║
	/// ║  └─ BuildSequence(): Task 조립              ║
	/// ╚══════════════════════════════════════════════╝
	/// 
	/// !!! 수정 가이드 !!!
	/// 1. GetSequenceName(): SampleZFocusSequenceBuilder::GetSequenceName()과 일치해야 함
	/// 2. CreateBuilder(): 장비별 시퀀스 빌더로 교체
	/// 3. ConfigureParams():
	///    - CameraIndex: 장비의 실제 Camera 번호로 변경
	///    - PickerMaxRow/Col: 장비의 Picker 최대 행/열로 변경
	///    - VisionPoint 좌표: DB에서 읽거나 고정값을 장비 좌표로 변경
	///    - Magic Number (위치 보정값): 장비의 캘리브레이션 값으로 변경
	/// </summary>
	class VMFEQUIPMENTPLUGIN_API SampleSequenceStrategy : public DefaultSetupStrategy
	{
	public:
		std::string GetSequenceName() const override { return "SampleZFocus"; }

		SequenceBuilderPtr CreateBuilder() override
		{
			return std::make_shared<SampleZFocusSequenceBuilder>();
		}

        void ConfigureParams(VMF::VisionContextPtr ctx) override
		{
			// =========================================================
			// [1] Task "SampleMoveToStartPositionTask" 전용 파라미터
			//     MoveToStartPosition Task가 사용하는 params
			// =========================================================
			{
				VisionParams moveParams;

				// 타임아웃 설정 (10초)
				moveParams.visionParams[VAT_SEQ_PARAM_TIMEOUT_MS] = "10000";

				// Database에서 검사 위치 로드 (샘플)
				// !!! 수정 필요: 장비의 DB 테이블/필드명으로 변경 !!!
				const int cameraIndex   = 6;
				const int packageId     = 1;

				// Load1 방식: DB에서 VisionPoint 좌표 읽기
				auto repo = ctx->GetRepository();
				if (repo)
				{
					double posX = 0.0, posY = 0.0, posZ = 0.0;
					repo->LoadInspInitPos(cameraIndex, 1, packageId, posX, posY, posZ);
				}

				// !!! 수정 필요: VisionPoint 좌표는 장비의 검사 위치로 변경 !!!
				AddVisionPoint(moveParams, 1, 1, 125.3, 48.7, -2.5);
				AddVisionPoint(moveParams, 2, 2, 130.1, 52.3, -1.8);
				AddVisionPoint(moveParams, 3, 3, 118.9, 45.6, -3.2);

				ctx->SetTaskParams("SampleMoveToStartPositionTask", moveParams);
			}

			// =========================================================
			// [2] Task "SamplePerformFocusScanningTask" 전용 파라미터
			//     PerformFocusScanning Task가 사용하는 params
			// =========================================================
			{
				VisionParams focusParams;

				const int cameraIndex   = 6;
				const int packageId     = 1;
				const int pickerMaxRow  = 4;
				const int pickerMaxCol  = 9;

				// 비전/시퀀스 파라미터
				focusParams.visionParams[VAT_SEQ_PARAM_CAMERA_INDEX] = std::to_string(cameraIndex);
				focusParams.visionParams[VAT_SEQ_PARAM_PACKAGE_ID] = std::to_string(packageId);
				focusParams.visionParams[VAT_SEQ_PARAM_MOTION_TIMEOUT_MS] = "7000";
				focusParams.visionParams[VAT_SEQ_PARAM_VISION_TIMEOUT_MS] = "30000";

				// 검사 위치 로드
				auto repo = ctx->GetRepository();
				if (repo)
				{
					double posX = 0.0, posY = 0.0, posZ = 0.0;
					repo->LoadInspInitPos(cameraIndex, 1, packageId, posX, posY, posZ);
				}

				// !!! 수정 필요: VisionPoint 좌표는 장비의 검사 위치로 변경 !!!
				AddVisionPoint(focusParams, 1, 1, 125.3, 48.7, -2.5);
				AddVisionPoint(focusParams, 2, 2, 130.1, 52.3, -1.8);
				AddVisionPoint(focusParams, 3, 3, 118.9, 45.6, -3.2);

				// !!! 수정 필요: CameraIndex가 5보다 큰 조건은 장비 Camera 구성에 따라 변경 !!!
				if (cameraIndex > 5)
				{
					AddVisionPoint(focusParams, 3, 3, 118.9, 45.6, -3.2);    // UpperTarget
					AddVisionPoint(focusParams, 4, 4, 122.4, 50.1, -2.1);    // TargetA
					AddVisionPoint(focusParams, 5, 5, 128.7, 47.9, -2.9);    // TargetB
				}

				// Picker 위치 (장비별 행/열 크기 확인 후 수정)
				// !!! 수정 필요: Picker 주소 및 행/열 개수는 장비에 맞게 변경 !!!
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

				ctx->SetTaskParams("SamplePerformFocusScanningTask", focusParams);
			}
		}
	};
} // namespace VMF_Sample
