#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1FindAlignPosTask : public DVH_VAT::NonBlockingTaskBase
        {
            enum Substep
            {
                MoveSafeZ = 0,
                MoveOrigin,
                MoveCalibrationPosXY,
                MoveFocusPosZ,
                VisionRequest,
                VisionWait,
                SaveCalibrationResult,
            };

            DVH_VAT::TaskResult HandleMoveSafeZ(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveOrigin(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveCalibrationXY(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleMoveFocusZ(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleVisionRequest(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleVisionWait(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleSaveCalibrationResult(DVH_VAT::VAT_Context& ctx);

		public:
			CLoad1FindAlignPosTask();
			virtual ~CLoad1FindAlignPosTask();

            std::string GetName() const override
            {
                return "Task_FindAlignPos";
            }

		protected:
            void OnInitialize(DVH_VAT::VAT_Context& ctx) override;
            DVH_VAT::TaskResult OnPoll(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator) override;

			std::vector<double> m_targetPosition;
			std::vector<DVH_VAT::VisionPosition> m_alignedPositions;

			int m_locationId;
			int m_packageId;
			int m_cameraId;

			int m_visionTimeoutMs;
			long m_moveTimeoutMs;

			int m_inspectionCount;
			int m_maxInspectionCount;
		};
    } // namespace Task
} // namespace VAT_LOAD1
