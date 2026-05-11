#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1PerformCalibrationTask : public VMF::NonBlockingTaskBase
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

            VMF::TaskResult HandleMoveSafeZ(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveOrigin(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveCalibrationPositionXY(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveFocusPositionZ(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleVisionRequest(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleVisionWait(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleSaveCalibrationResult(VMF::VAT_Context& ctx);

		public:
			CLoad1PerformCalibrationTask();
			virtual ~CLoad1PerformCalibrationTask();

            std::string GetName() const override
            {
                return "Task_PerformCalibration";
            }

		protected:
            void OnInitialize(VMF::VAT_Context& ctx) override;
            VMF::TaskResult OnPoll(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator) override;

			int m_locationId;
			int m_packageId;
			int m_cameraId;
			std::vector<double> m_targetPosition;
			int m_visionTimeoutMs;
			long m_moveTimeoutMs;
			int m_inspectionCount;
			int m_maxInspectionCount;
            VMF::PitchStatus m_currentPitchMode;
			bool m_isWideCheck;
		};
    } // namespace Task
} // namespace VAT_LOAD1
