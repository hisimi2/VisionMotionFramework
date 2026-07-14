#pragma once

#include "NonBlockingTaskBase.h"

namespace VMF_PLUGIN
{

	class CLoad1VATPerformFocusScanningTask : public VMF::NonBlockingTaskBase
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
		CLoad1VATPerformFocusScanningTask();
		virtual ~CLoad1VATPerformFocusScanningTask();

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
}
