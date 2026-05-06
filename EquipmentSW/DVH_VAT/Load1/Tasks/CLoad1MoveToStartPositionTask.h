#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class VAT_Context;
		class IVatActuator;
		class IDataRepository;

		class CLoad1MoveToStartPositionTask : public DVH_VAT::NonBlockingTaskBase
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

            DVH_VAT::TaskResult HandleMoveSafeZ(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveOrigin(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveTargetPositionXY(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveTargetPositionZ(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleCompleteMove(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);

		public:
			CLoad1MoveToStartPositionTask();
			virtual ~CLoad1MoveToStartPositionTask();

            std::string GetName() const override
            {
                return "Task_MoveToStartPosition";
            }

		protected:
            void OnInitialize(DVH_VAT::VAT_Context& ctx) override;
            DVH_VAT::TaskResult OnPoll(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator) override;

            std::vector<double> m_targetPosition;
            long m_moveTimeoutMs;
		};
    } // namespace Task
} // namespace VAT_LOAD1
