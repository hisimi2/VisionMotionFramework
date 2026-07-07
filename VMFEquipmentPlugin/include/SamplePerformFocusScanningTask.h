#pragma once

#include "NonBlockingTaskBase.h"
#include "VMFEquipmentPluginExport.h"

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
		class VMFEQUIPMENTPLUGIN_API SamplePerformFocusScanningTask : public VMF::NonBlockingTaskBase
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

			/// <summary>
			/// Builder에서 Task-specific 파라미터를 주입합니다.
			/// </summary>
			void SetTaskParams(const VMF::VisionParams& params) { m_taskParams = params; }

		protected:
			void OnInitialize(VMF::Context& ctx) override;
			VMF::TaskResult OnPoll(VMF::Context& ctx, VMF::IActuator* actuator) override;

			/// <summary>
			/// Task-specific 파라미터를 읽습니다.
			/// m_taskParams.visionParams에서 키를 찾고, 없으면 defaultValue 반환
			/// </summary>
			template <typename T>
			T GetTaskSeqParamAs(VMF::Context& ctx, const std::string& key, const T& defaultValue)
			{
				(void)ctx;
				auto it = m_taskParams.visionParams.find(key);
				if (it != m_taskParams.visionParams.end())
				{
					T converted;
					if (VMF::detail::ParamConverter<T>::Convert(it->second, converted))
						return converted;
				}
				return defaultValue;
			}

			/// <summary>
			/// Task-specific VisionPosition 목록에서 첫 번째 항목을 제거하지 않고 조회합니다.
			/// </summary>
			bool PeekTaskVisionPosition(VMF::VisionPosition& outPos)
			{
				if (m_taskParams.visionPositions.empty())
					return false;
				outPos = m_taskParams.visionPositions.front();
				return true;
			}

			/// <summary>
			/// Task-specific VisionPosition 목록에서 첫 번째 항목을 꺼내 반환하고 제거합니다.
			/// </summary>
			bool PopTaskVisionPosition(VMF::VisionPosition& outPos)
			{
				if (m_taskParams.visionPositions.empty())
					return false;
				outPos = m_taskParams.visionPositions.front();
				m_taskParams.visionPositions.erase(m_taskParams.visionPositions.begin());
				return true;
			}

			/// <summary>
			/// Task-specific VisionPosition 목록이 비어 있는지 확인합니다.
			/// </summary>
			bool IsTaskVisionPositionEmpty() const
			{
				return m_taskParams.visionPositions.empty();
			}

			int m_cameraId;
			int m_packageId;
			std::vector<int> m_locationIds;
			long m_moveTimeoutMs;
			int m_visionTimeoutMs;

			VMF::VisionParams m_taskParams;
		};
	} // namespace Task
} // namespace VMF_Sample
