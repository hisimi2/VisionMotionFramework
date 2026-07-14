#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1VATPerformPickerFOVScanningTask : public VMF::NonBlockingTaskBase
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

            VMF::TaskResult HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMoveOrigin(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMoveScanPosition(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMoveFocusPositionZ(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleVisionRequest(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleVisionWait(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleSaveResult(VMF::Context& ctx);
            VMF::TaskResult HandleReturnHome(VMF::Context& ctx, VMF::IActuator* actuator);

		public:
			CLoad1VATPerformPickerFOVScanningTask();
			virtual ~CLoad1VATPerformPickerFOVScanningTask();

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
			void OnInitialize(VMF::Context& ctx) override;
			VMF::TaskResult OnPoll(VMF::Context& ctx, VMF::IActuator* actuator) override;

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
