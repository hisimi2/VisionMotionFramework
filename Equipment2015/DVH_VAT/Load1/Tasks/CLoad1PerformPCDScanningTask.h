#pragma once
#include "NonBlockingTaskBase.h"
#include <string>

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1PerformPCDScanningTask : public DVH_VAT::NonBlockingTaskBase
		{
            enum Substep
            {
                MoveSafeZ = 0,
                MoveOrigin,
                MoveTargetPositionXY,
                MoveFocusPositionZ,
                VisionRequest,
                VisionWait,
                CalculatePCD,
                SavePCDResult
            };

            DVH_VAT::TaskResult HandleMoveSafeZ(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveOrigin(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveTargetPositionXY(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveFocusPositionZ(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleVisionRequest(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleVisionWait(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleCalculatePCD(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleSavePCDResult(DVH_VAT::VAT_Context& ctx);

		public:
			CLoad1PerformPCDScanningTask();
			virtual ~CLoad1PerformPCDScanningTask();

            std::string GetName() const override
            {
                return "Task_PerformPCDScanning";
            }

		protected:
            void OnInitialize(DVH_VAT::VAT_Context& ctx) override;
            DVH_VAT::TaskResult OnPoll(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator) override;

            enum Axis
            {
                Axis_X = 0,
                Axis_Y
            };

            Axis m_scanAxis;
			double m_scanPitch;
			double m_startAxisPulse;
			double m_endAxisPulse;
			double m_targetPositionX;
			double m_targetPositionY;
			double m_targetTablePosition;
			double m_focusPositionZ;
			double m_pulseDistanceResult;
			double m_scaleResult;
			long m_moveTimeoutMs;
			int m_visionTimeoutMs;
			bool m_isStartOffsetMeasured;
			int m_inspectionCount;
			int m_maxInspectionCount;
			int m_cameraId;
		};
    } // namespace Task
} // namespace VAT_LOAD1
