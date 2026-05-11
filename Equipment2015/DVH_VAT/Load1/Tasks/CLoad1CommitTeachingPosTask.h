#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1CommitTeachingPosTask : public VMF::NonBlockingTaskBase
		{
            enum Substep
            {
                LoadPickerCameraOffset = 0,
                LoadVisionPositions,
                LoadHandPitchOffsets,
                SaveTeachingPositions
            };

            VMF::TaskResult HandleLoadPickerCameraOffset(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleLoadVisionPositions(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleLoadHandPitchOffsets(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleSaveTeachingPositions(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);

		public:
			CLoad1CommitTeachingPosTask();
			virtual ~CLoad1CommitTeachingPosTask();

            std::string GetName() const override
            {
                return "Task_CommitTeachingPos";
            }

		protected:
            void OnInitialize(VMF::VAT_Context& ctx) override;
            VMF::TaskResult OnPoll(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator) override;

            struct HandPitchOffset
            {
                int row;
                int col;
                double narrowOffsetX;
                double narrowOffsetY;
                double wideOffsetX;
                double wideOffsetY;

                HandPitchOffset()
                    : row(0)
                    , col(0)
                    , narrowOffsetX(0.0)
                    , narrowOffsetY(0.0)
                    , wideOffsetX(0.0)
                    , wideOffsetY(0.0)
                {
                }
            };

			double m_pickerCameraOffsetX_Narrow;
			double m_pickerCameraOffsetY_Narrow;
			double m_pickerCameraOffsetX_Wide;
			double m_pickerCameraOffsetY_Wide;
			std::vector<VMF::VisionPosition> m_teachingPositions;
			std::vector<HandPitchOffset> m_handPitchOffsets;
		};
    } // namespace Task
} // namespace VAT_LOAD1 
