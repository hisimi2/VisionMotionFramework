#pragma once
#include "NonBlockingTaskBase.h"
#include <boost/chrono.hpp>
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
            DVH_VAT::PitchStatus pitchMode;
			double targetPositionX;
			double targetPositionY;
			double measuredOffsetX;
			double measuredOffsetY;

			HandPitchScanPoint()
				: row(0)
				, col(0)
				, pitchMode(DVH_VAT::Narrow)
				, targetPositionX(0.0)
				, targetPositionY(0.0)
				, measuredOffsetX(0.0)
				, measuredOffsetY(0.0)
			{
			}
		};

		class CLoad1PerformHandPitchScanningTask : public DVH_VAT::NonBlockingTaskBase
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

            DVH_VAT::TaskResult HandleMoveSafeZ(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveOrigin(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveHandPitch(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveFocusPositionZ(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleVisionRequest(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleVisionWait(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleReturnHome(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleSaveHandPitchResult(DVH_VAT::VAT_Context& ctx);

            void BuildScanPoints(
                double standardPickerCol,
                double standardPickerRow,
                double pickerGapX_Narrow,
                double pickerGapX_Wide,
                double pickerGapY_Narrow,
                double pickerGapY_Wide);

		public:
			CLoad1PerformHandPitchScanningTask();
			virtual ~CLoad1PerformHandPitchScanningTask();

            std::string GetName() const override
            {
                return "Task_PerformHandPitchScanning";
            }

		protected:
            void OnInitialize(DVH_VAT::VAT_Context& ctx) override;
            DVH_VAT::TaskResult OnPoll(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator) override;

            size_t m_currentScanIndex;
			double m_centerPositionX;
			double m_centerPositionY;
			double m_focusPositionZ;
			double m_safePositionZ;
			int m_cameraId;
			double m_currentTargetPositionX;
			double m_currentTargetPositionY;
			std::vector<HandPitchScanPoint> m_scanPoints;
            DVH_VAT::PitchStatus m_currentPitchMode;
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
