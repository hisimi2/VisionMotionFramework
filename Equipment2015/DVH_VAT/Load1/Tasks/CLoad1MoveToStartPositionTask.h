#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
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

            VMF::TaskResult HandleMoveSafeZ(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveOrigin(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveTargetPositionXY(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveTargetPositionZ(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleCompleteMove(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);

		public:
			CLoad1MoveToStartPositionTask();
			virtual ~CLoad1MoveToStartPositionTask();

            std::string GetName() const override
            {
                return "Task_MoveToStartPosition";
            }

		protected:
            void OnInitialize(VMF::VAT_Context& ctx) override;
            VMF::TaskResult OnPoll(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator) override;

            std::vector<double> m_targetPosition;
            long m_moveTimeoutMs;
		};
    } // namespace Task
} // namespace VAT_LOAD1
