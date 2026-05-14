#pragma once
#include "NonBlockingTaskBase.h"
#include <string>

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1PerformPCDScanningTask : public VMF::NonBlockingTaskBase
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

            VMF::TaskResult HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMoveOrigin(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMoveTargetPositionXY(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMoveFocusPositionZ(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleVisionRequest(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleVisionWait(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleCalculatePCD(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleSavePCDResult(VMF::Context& ctx);

		public:
			CLoad1PerformPCDScanningTask();
			virtual ~CLoad1PerformPCDScanningTask();

            std::string GetName() const override
            {
                return "Task_PerformPCDScanning";
            }

		protected:
            void OnInitialize(VMF::Context& ctx) override;
            VMF::TaskResult OnPoll(VMF::Context& ctx, VMF::IActuator* actuator) override;

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
