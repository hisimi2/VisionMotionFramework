#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1PerformFocusScanningTask : public DVH_VAT::NonBlockingTaskBase
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

            DVH_VAT::TaskResult HandleMoveDown(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveWait(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleVisionRequest(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleVisionWait(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleReturnHome(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleSaveFocusResult(DVH_VAT::VAT_Context& ctx);

		public:
			CLoad1PerformFocusScanningTask();
			virtual ~CLoad1PerformFocusScanningTask();

            std::string GetName() const override
            {
                return "Task_PerformFocusScanning";
            }

		protected:
            void OnInitialize(DVH_VAT::VAT_Context& ctx) override;
            DVH_VAT::TaskResult OnPoll(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator) override;

			int m_cameraId;
			int m_packageId;
			std::vector<int> m_locationIds;
			long m_moveTimeoutMs;
			int m_visionTimeoutMs;
		};
    } // namespace Task
} // namespace VAT_LOAD1
