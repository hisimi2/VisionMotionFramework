#pragma once

#include "NonBlockingTaskBase.h"

namespace VMF_Sample
{
	namespace Task
	{
		/// <summary>
		/// [Sample] Focus Scanning 수행 Task
		/// Z축 하강 → Vision 촬영 요청 → 결과 수신 → 복귀 → DB 저장 순서로 진행
		/// 
		/// !!! 수정 가이드 !!!
		/// 1. m_cameraId, m_packageId: ConfigureParams에서 설정된 값을 사용 (또는 고정값 변경)
		/// 2. HandleSaveFocusResult: DB 저장 로직은 장비의 DB 테이블 구조에 맞게 수정
		/// 3. VisionCommand 종류: VisionCommands::Measure 외에 다른 명령이 필요하면 변경
		/// 4. 조명 제어: actuator->SetLightState()는 장비의 조명 제어 방식에 맞게 수정
		/// </summary>
		class SamplePerformFocusScanningTask : public VMF::NonBlockingTaskBase
		{
			enum Substep
			{
				MoveDown = 0,
				MoveWait,
				VisionRequest,
				VisionWait,
				ReturnHome,
				SaveFocusResult,
			};

			VMF::TaskResult HandleMoveDown(VMF::Context& ctx, VMF::IActuator* actuator);
			VMF::TaskResult HandleMoveWait(VMF::Context& ctx, VMF::IActuator* actuator);
			VMF::TaskResult HandleVisionRequest(VMF::Context& ctx, VMF::IActuator* actuator);
			VMF::TaskResult HandleVisionWait(VMF::Context& ctx, VMF::IActuator* actuator);
			VMF::TaskResult HandleReturnHome(VMF::Context& ctx, VMF::IActuator* actuator);
			VMF::TaskResult HandleSaveFocusResult(VMF::Context& ctx);

		public:
			SamplePerformFocusScanningTask();
			virtual ~SamplePerformFocusScanningTask();

			std::string GetName() const override
			{
				return "Task_PerformFocusScanning";
			}

		protected:
			void OnInitialize(VMF::Context& ctx) override;
			VMF::TaskResult OnPoll(VMF::Context& ctx, VMF::IActuator* actuator) override;

			int m_cameraId;
			int m_packageId;
			std::vector<int> m_locationIds;
			long m_moveTimeoutMs;
			int m_visionTimeoutMs;
		};
	} // namespace Task
} // namespace VMF_Sample
