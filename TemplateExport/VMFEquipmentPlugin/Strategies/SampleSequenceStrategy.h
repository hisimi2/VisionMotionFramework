#pragma once
#include "DefaultSetupStrategy.h"
#include "Sequences/SampleZFocusSequenceBuilder.h"

namespace VMF_Sample
{
	using namespace VMF;
	using namespace Sequence;

	/// <summary>
	/// [Sample] Focus Check Sequence 전략 클래스
	/// DefaultSetupStrategy를 상속받아 시퀀스 빌더 생성과 파라미터 설정을 담당
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
	class SampleSequenceStrategy : public DefaultSetupStrategy
	{
	public:
		std::string GetSequenceName() const override { return "SampleZFocus"; }

		SequenceBuilderPtr CreateBuilder() override
		{
			return std::make_shared<SampleZFocusSequenceBuilder>();
		}

		void ConfigureParams(VMF::VisionContextPtr ctx) override
		{
			// [SAMPLE] Camera/Picker 파라미터 설정
			// !!! 수정 필요: 장비의 실제 값으로 변경 !!!
			const int cameraIndex = 6;
			const int pickerMaxRow = 4;
			const int pickerMaxCol = 9;
			const int packageId = 1;

			// Database에서 검사 위치 로드 (샘플)
			// !!! 수정 필요: 장비의 DB 테이블/필드명으로 변경 !!!
			{
				double posX = 0.0, posY = 0.0, posZ = 0.0;

				// Load1 방식: DB에서 VisionPoint 좌표 읽기
				auto repo = ctx->GetRepository();
				if (repo)
				{
					repo->LoadInspInitPos(cameraIndex, 1, packageId, posX, posY, posZ);
				}

				// [SAMPLE] CameraIndex=6, LocateId=1, PackageId=1
				// posX=125.3, posY=48.7, posZ=-2.5

				// !!! 수정 필요: cameraIndex, locateId, packageId는 장비 설정에 맞게 변경 !!!
				SetParam(VAT_SEQ_PARAM_CAMERA_INDEX, cameraIndex);
				SetParam(VAT_SEQ_PARAM_PACKAGE_ID, packageId);

				// !!! 수정 필요: VisionPoint 좌표는 장비의 검사 위치로 변경 !!!
				AddVisionPoint(1, 125.3, 48.7, -2.5);   // LocateId=1
				AddVisionPoint(2, 130.1, 52.3, -1.8);   // LocateId=2
				AddVisionPoint(3, 118.9, 45.6, -3.2);   // LocateId=3
			}

			// !!! 수정 필요: CameraIndex가 5보다 큰 조건은 장비 Camera 구성에 따라 변경 !!!
			if (cameraIndex > 5)
			{
				// [SAMPLE] 상부/하부 Target 보정값 (장비별 캘리브레이션 값)
				AddVisionPoint(3, 118.9, 45.6, -3.2);    // UpperTarget
				AddVisionPoint(4, 122.4, 50.1, -2.1);    // TargetA
				AddVisionPoint(5, 128.7, 47.9, -2.9);    // TargetB
			}

			// Picker 위치 (장비별 행/열 크기 확인 후 수정)
			// !!! 수정 필요: Picker 주소(Addr20, Addr21) 및 행/열 개수는 장비에 맞게 변경 !!!
			for (int row = 0; row < pickerMaxRow; ++row)
			{
				for (int col = 0; col < pickerMaxCol; ++col)
				{
					double pickerX = 200.0 + col * 25.0;
					double pickerY = 100.0 + row * 30.0;
					double pickerZ = -5.0;

					// !!! 수정 필요: LocateId 계산식은 장비의 Picker 매핑에 맞게 변경 !!!
					int locateId = 20 + row * 10 + col;
					AddVisionPoint(locateId, pickerX, pickerY, pickerZ);
				}
			}

			// [SAMPLE] 기본 Sequence 파라미터
			SetParam(VAT_SEQ_PARAM_MOTION_TIMEOUT_MS, 7000);
			SetParam(VAT_SEQ_PARAM_VISION_TIMEOUT_MS, 60000);
		}
	};
} // namespace VMF_Sample
