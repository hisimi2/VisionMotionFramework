#pragma once
#include "NonBlockingTaskBase.h"
#include "chrono"
#include <string>
#include <vector>

namespace VAT_LOAD1
{
	namespace Task
	{
		struct HandPitchScanPoint
		{
			int row;
			int col;
            VMF::PitchStatus pitchMode;
			double targetPositionX;
			double targetPositionY;
			double measuredOffsetX;
			double measuredOffsetY;

			HandPitchScanPoint()
				: row(0)
				, col(0)
				, pitchMode(VMF::Narrow)
				, targetPositionX(0.0)
				, targetPositionY(0.0)
				, measuredOffsetX(0.0)
				, measuredOffsetY(0.0)
			{
			}
		};

		class CLoad1VATPerformHandPitchScanningTask : public VMF::NonBlockingTaskBase
        {
            enum Substep
            {
                MoveSafeZ = 0,
                MoveOrigin,
                MoveHandPitch,
                MoveFocusPositionZ,
                VisionRequest,
                VisionWait,
                ReturnHome,
                SaveHandPitchResult,
            };

            VMF::TaskResult HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMoveOrigin(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMoveHandPitch(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMoveFocusPositionZ(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleVisionRequest(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleVisionWait(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleReturnHome(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleSaveHandPitchResult(VMF::Context& ctx);

            void BuildScanPoints(
                double standardPickerCol,
                double standardPickerRow,
                double pickerGapX_Narrow,
                double pickerGapX_Wide,
                double pickerGapY_Narrow,
                double pickerGapY_Wide);

		public:
			CLoad1VATPerformHandPitchScanningTask();
			virtual ~CLoad1VATPerformHandPitchScanningTask();

            std::string GetName() const override
            {
                return "Task_PerformHandPitchScanning";
            }

		protected:
            void OnInitialize(VMF::Context& ctx) override;
            VMF::TaskResult OnPoll(VMF::Context& ctx, VMF::IActuator* actuator) override;

            size_t m_currentScanIndex;
			double m_centerPositionX;
			double m_centerPositionY;
			double m_focusPositionZ;
			double m_safePositionZ;
			int m_cameraId;
			double m_currentTargetPositionX;
			double m_currentTargetPositionY;
			std::vector<HandPitchScanPoint> m_scanPoints;
            VMF::PitchStatus m_currentPitchMode;
			int m_pickerMaxRow;
			int m_pickerMaxCol;
			int m_visionTimeoutMs;
			long m_moveTimeoutMs;

			double m_standardPickerCol;
			double m_standardPickerRow;
			double m_pickerGapX_Narrow;
			double m_pickerGapX_Wide;
			double m_pickerGapY_Narrow;
			double m_pickerGapY_Wide;
			bool m_isScanPointsBuilt;
		};
    } // namespace Task
} // namespace VAT_LOAD1 
