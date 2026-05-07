#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1PerformPickerFOVScanningTask : public DVH_VAT::NonBlockingTaskBase
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

            DVH_VAT::TaskResult HandleMoveSafeZ(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveOrigin(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveScanPosition(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveFocusPositionZ(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleVisionRequest(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleVisionWait(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleSaveResult(DVH_VAT::VAT_Context& ctx);
            DVH_VAT::TaskResult HandleReturnHome(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);

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
			void OnInitialize(DVH_VAT::VAT_Context& ctx) override;
			DVH_VAT::TaskResult OnPoll(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator) override;

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
