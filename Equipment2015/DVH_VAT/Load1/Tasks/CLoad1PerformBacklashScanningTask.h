#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1PerformBacklashScanningTask : public VMF::NonBlockingTaskBase
		{
            enum Substep
            {
                MoveSafeZ = 0,
                MoveOrigin,
                MoveStart,
                MoveZero,
                MoveEnd,
                MoveFocusZ,
                VisionRequest,
                VisionWait,
                CalculateBacklash,
                SaveBacklashResult
            };

            VMF::TaskResult HandleMoveSafeZ(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveOrigin(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveStart(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveZero(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveEnd(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveFocusZ(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleVisionRequest(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleVisionWait(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleCalculateBacklash(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleSaveBacklashResult(VMF::VAT_Context& ctx);

		public:
			CLoad1PerformBacklashScanningTask();
			virtual ~CLoad1PerformBacklashScanningTask();

            std::string GetName() const override
            {
                return "Task_PerformBacklashScanning";
            }

		protected:
            void OnInitialize(VMF::VAT_Context& ctx) override;
            VMF::TaskResult OnPoll(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator) override;

            enum Axis
            {
                Axis_X = 0,
                Axis_Y
            };

			struct SpeedInfo
			{
				int speed;
				int acc;
				int dcc;
			};

            void ApplyCurrentSpeed(VMF::IVatActuator* actuator);

			int m_cameraId;
			double m_zeroPositionX;
			double m_zeroPositionY;
			double m_startPositionX;
			double m_startPositionY;
			double m_endPositionX;
			double m_endPositionY;
			double m_targetTablePosition;
            Axis m_scanAxis;
			double m_currentPositionX;
			double m_currentPositionY;
			double m_focusPositionZ;
			double m_startOffsetX;
			double m_startOffsetY;
			double m_endOffsetX;
			double m_endOffsetY;

			std::vector<SpeedInfo> m_speedList;
			int m_currentSpeedIndex;
			std::vector<double> m_backlashResults;
			bool m_isStartMeasured;
			int m_nextStateAfterSafeZ;
			long m_visionTimeoutMs;
			long m_moveTimeoutMs;
		};
    } // namespace Task
} // namespace VAT_LOAD1
