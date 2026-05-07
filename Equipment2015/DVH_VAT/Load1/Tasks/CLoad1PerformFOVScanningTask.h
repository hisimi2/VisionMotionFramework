#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1PerformFOVScanningTask : public DVH_VAT::NonBlockingTaskBase
        {
            enum Substep
            {
                MoveSafeZ = 0,
                MoveOrigin,
                MoveScanPosition,
                MoveFocusPositionZ,
                VisionRequest,
                VisionWait,
                ReturnHome,
            };

            DVH_VAT::TaskResult HandleMoveSafeZ(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveOrigin(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveScanPosition(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveFocusPositionZ(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleVisionRequest(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleVisionWait(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleReturnHome(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);

		public:
			CLoad1PerformFOVScanningTask();
			virtual ~CLoad1PerformFOVScanningTask();

            std::string GetName() const override
            {
                return "Task_PerformFOVScanning";
            }

			enum FOVDirection
			{
				CENTER = 0,
				REAR,
				FRONT,
				LEFT_SIDE,
				RIGHT_SIDE
			};

		protected:
            void OnInitialize(DVH_VAT::VAT_Context& ctx) override;
            DVH_VAT::TaskResult OnPoll(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator) override;

			FOVDirection m_currentScanDirection;
			double m_centerPositionX;
			double m_centerPositionY;
			double m_focusPositionZ;
			int m_cameraId;
			double m_targetPositionX;
			double m_targetPositionY;
			double m_fovInterval;
			int m_visionTimeoutMs;
			long m_moveTimeoutMs;
		};
    } // namespace Task
} // namespace VAT_LOAD1 
