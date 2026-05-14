#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1PerformFocusScanningTask : public VMF::NonBlockingTaskBase
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
			CLoad1PerformFocusScanningTask();
			virtual ~CLoad1PerformFocusScanningTask();

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
} // namespace VAT_LOAD1
