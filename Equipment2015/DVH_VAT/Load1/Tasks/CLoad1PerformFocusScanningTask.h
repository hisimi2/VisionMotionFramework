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

            VMF::TaskResult HandleMoveDown(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveWait(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleVisionRequest(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleVisionWait(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleReturnHome(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleSaveFocusResult(VMF::VAT_Context& ctx);

		public:
			CLoad1PerformFocusScanningTask();
			virtual ~CLoad1PerformFocusScanningTask();

            std::string GetName() const override
            {
                return "Task_PerformFocusScanning";
            }

		protected:
            void OnInitialize(VMF::VAT_Context& ctx) override;
            VMF::TaskResult OnPoll(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator) override;

			int m_cameraId;
			int m_packageId;
			std::vector<int> m_locationIds;
			long m_moveTimeoutMs;
			int m_visionTimeoutMs;
		};
    } // namespace Task
} // namespace VAT_LOAD1
