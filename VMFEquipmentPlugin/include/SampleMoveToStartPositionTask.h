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

protected:
		void OnInitialize(VMF::Context& ctx) override;
		VMF::TaskResult OnPoll(VMF::Context& ctx, VMF::IActuator* actuator) override;

		std::vector<double> m_targetPosition;
		long m_moveTimeoutMs;
		};
	} // namespace Task
} // namespace VMF_Sample
