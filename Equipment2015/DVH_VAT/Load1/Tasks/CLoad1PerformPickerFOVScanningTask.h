#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1PerformPickerFOVScanningTask : public VMF::NonBlockingTaskBase
        {
            enum Substep
            {
                MoveSafeZ = 0,
                MoveOrigin,
                MoveScanPosition,
                MoveFocusPositionZ,
                VisionRequest,
                VisionWait,
                SaveResult,
                ReturnHome
            };

            VMF::TaskResult HandleMoveSafeZ(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveOrigin(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveScanPosition(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveFocusPositionZ(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleVisionRequest(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleVisionWait(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleSaveResult(VMF::VAT_Context& ctx);
            VMF::TaskResult HandleReturnHome(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);

		public:
			CLoad1PerformPickerFOVScanningTask();
			virtual ~CLoad1PerformPickerFOVScanningTask();

            std::string GetName() const override
            {
                return "Task_PerformPickerFOVScanning";
            }

			enum FOVDirection
			{
				CENTER = 0,
				FRONT_RIGHT
			};

		protected:
			void OnInitialize(VMF::VAT_Context& ctx) override;
			VMF::TaskResult OnPoll(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator) override;

			FOVDirection m_currentScanDirection;

			double m_centerPositionX;
			double m_centerPositionY;
			double m_focusPositionZ;
			// target position은 상태간 유지할 필요 없음 -> 지역변수로 변경

			double m_fovIntervalX;
			double m_fovIntervalY;

			double m_centerOffsetX;
			double m_centerOffsetY;
			double m_frontRightOffsetX;
			double m_frontRightOffsetY;

			int m_cameraId;
			int m_packageId;
			int m_visionTimeoutMs;
			long m_moveTimeoutMs;
		};
    }
}
