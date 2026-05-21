#pragma once

#include "NonBlockingTaskBase.h"

namespace VMF_Load1
{
	namespace Task
	{
		class VAT_Context;
		class IVatActuator;
		class IDataRepository;

		class CLoad1MoveToStartPositionTask : public VMF::NonBlockingTaskBase
		{
            enum Substep
            {
                Idle = 0,
                MoveSafeZ,
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
			CLoad1MoveToStartPositionTask();
			virtual ~CLoad1MoveToStartPositionTask();

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
} // namespace VMF_Load1
