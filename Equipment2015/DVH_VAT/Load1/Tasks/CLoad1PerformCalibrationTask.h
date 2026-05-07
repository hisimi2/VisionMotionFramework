#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1PerformCalibrationTask : public DVH_VAT::NonBlockingTaskBase
        {
            enum Substep
            {
                MoveSafeZ = 0,
                MoveOrigin,
                MoveCalibrationPositionXY,
                MoveFocusPositionZ,
                VisionRequest,
                VisionWait,
                SaveCalibrationResult,
            };

            DVH_VAT::TaskResult HandleMoveSafeZ(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveOrigin(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveCalibrationPositionXY(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveFocusPositionZ(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleVisionRequest(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleVisionWait(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleSaveCalibrationResult(DVH_VAT::VAT_Context& ctx);

		public:
			CLoad1PerformCalibrationTask();
			virtual ~CLoad1PerformCalibrationTask();

            std::string GetName() const override
            {
                return "Task_PerformCalibration";
            }

		protected:
            void OnInitialize(DVH_VAT::VAT_Context& ctx) override;
            DVH_VAT::TaskResult OnPoll(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator) override;

			int m_locationId;
			int m_packageId;
			int m_cameraId;
			std::vector<double> m_targetPosition;
			int m_visionTimeoutMs;
			long m_moveTimeoutMs;
			int m_inspectionCount;
			int m_maxInspectionCount;
            DVH_VAT::PitchStatus m_currentPitchMode;
			bool m_isWideCheck;
		};
    } // namespace Task
} // namespace VAT_LOAD1
