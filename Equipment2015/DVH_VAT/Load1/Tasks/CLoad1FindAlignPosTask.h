#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1FindAlignPosTask : public VMF::NonBlockingTaskBase
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

            VMF::TaskResult HandleMoveSafeZ(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveOrigin(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveCalibrationXY(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleMoveFocusZ(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleVisionRequest(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleVisionWait(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleSaveCalibrationResult(VMF::VAT_Context& ctx);

		public:
			CLoad1FindAlignPosTask();
			virtual ~CLoad1FindAlignPosTask();

            std::string GetName() const override
            {
                return "Task_FindAlignPos";
            }

		protected:
            void OnInitialize(VMF::VAT_Context& ctx) override;
            VMF::TaskResult OnPoll(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator) override;

			std::vector<double> m_targetPosition;
			std::vector<VMF::VisionPosition> m_alignedPositions;

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
