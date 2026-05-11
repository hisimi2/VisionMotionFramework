#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1PerformFOVScanningTask : public VMF::NonBlockingTaskBase
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

            VMF::TaskResult HandleMoveSafeZ(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveOrigin(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveScanPosition(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveFocusPositionZ(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleVisionRequest(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleVisionWait(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleReturnHome(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);

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
            void OnInitialize(VMF::VAT_Context& ctx) override;
            VMF::TaskResult OnPoll(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator) override;

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
