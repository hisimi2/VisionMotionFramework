#pragma once

#include "NonBlockingTaskBase.h"

namespace VMF_PLUGIN
{

	class Context;
	class IActuator;
	class IDataRepository;

	class CLoad1VATMoveToStartPositionTask : public VMF::NonBlockingTaskBase
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
		CLoad1VATMoveToStartPositionTask();
		virtual ~CLoad1VATMoveToStartPositionTask();

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
} // namespace VMF_PLUGIN
