#pragma once

#include "NonBlockingTaskBase.h"
#include "VMFEquipmentPluginExport.h"

namespace VMF_Sample
{
	namespace Task
	{
		/// <summary>
		/// [Sample] 검사 시작 위치로 이동하는 Task
		/// SafeZ 상승 → Origin 이동 → Target Position (XY) → Target Position (Z) 순서로 진행
		/// 
		/// !!! 수정 가이드 !!!
		/// 1. HandleMoveSafeZ: 장비의 SafeZ 높이/축을 실제 값으로 변경
		/// 2. HandleMoveOrigin: Origin 좌표/방식을 장비에 맞게 수정
		/// 3. HandleMoveTargetPositionXY/HandleMoveTargetPositionZ: Target 위치를 Context에서 읽거나 고정값으로 변경
		/// 4. 축 이름 (X/Y/Z/TABLE1/TABLE2) 은 장비의 실제 축 구성에 맞게 수정
		/// </summary>
		class VMFEQUIPMENTPLUGIN_API SampleMoveToStartPositionTask : public VMF::NonBlockingTaskBase
		{
			enum Substep
			{
				MoveSafeZ = 0,
				MoveOrigin,
				MoveTargetPositionXY,
				MoveTargetPositionZ,
				CompleteMove,
			};

			VMF::TaskResult HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator);
			VMF::TaskResult HandleMoveOrigin(VMF::Context& ctx, VMF::IActuator* actuator);
			VMF::TaskResult HandleMoveTargetPositionXY(VMF::Context& ctx, VMF::IActuator* actuator);
			VMF::TaskResult HandleMoveTargetPositionZ(VMF::Context& ctx, VMF::IActuator* actuator);
			VMF::TaskResult HandleCompleteMove(VMF::Context& ctx, VMF::IActuator* actuator);

		public:
			SampleMoveToStartPositionTask();
			virtual ~SampleMoveToStartPositionTask();

			std::string GetName() const override
			{
				return "Task_MoveToStartPosition";
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

			std::vector<double> m_targetPosition;
			long m_moveTimeoutMs;

			VMF::VisionParams m_taskParams;
		};
	} // namespace Task
} // namespace VMF_Sample
