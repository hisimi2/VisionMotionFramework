#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1VATFindAlignPosTask : public VMF::NonBlockingTaskBase
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

            VMF::TaskResult HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMoveOrigin(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMoveCalibrationXY(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMoveFocusZ(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleVisionRequest(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleVisionWait(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleSaveCalibrationResult(VMF::Context& ctx);

		public:
			CLoad1VATFindAlignPosTask();
			virtual ~CLoad1VATFindAlignPosTask();

            std::string GetName() const override
            {
                return "Task_FindAlignPos";
            }

		protected:
            void OnInitialize(VMF::Context& ctx) override;
            VMF::TaskResult OnPoll(VMF::Context& ctx, VMF::IActuator* actuator) override;

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
