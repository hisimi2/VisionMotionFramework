#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1PerformBacklashScanningTask : public DVH_VAT::NonBlockingTaskBase
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

            DVH_VAT::TaskResult HandleMoveSafeZ(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveOrigin(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveStart(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveZero(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveEnd(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveFocusZ(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleVisionRequest(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleVisionWait(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleCalculateBacklash(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleSaveBacklashResult(DVH_VAT::VAT_Context& ctx);

		public:
			CLoad1PerformBacklashScanningTask();
			virtual ~CLoad1PerformBacklashScanningTask();

            std::string GetName() const override
            {
                return "Task_PerformBacklashScanning";
            }

		protected:
            void OnInitialize(DVH_VAT::VAT_Context& ctx) override;
            DVH_VAT::TaskResult OnPoll(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator) override;

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

            void ApplyCurrentSpeed(DVH_VAT::IVatActuator* actuator);

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
