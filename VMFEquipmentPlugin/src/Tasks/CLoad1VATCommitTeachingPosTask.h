#pragma once

#include "NonBlockingTaskBase.h"

namespace VMF_PLUGIN
{

	class CLoad1VATCommitTeachingPosTask : public VMF::NonBlockingTaskBase
	{
        enum Substep
        {
            LoadPickerCameraOffset = 0,
            LoadVisionPositions,
            LoadHandPitchOffsets,
            SaveTeachingPositions
        };

        VMF::TaskResult HandleLoadPickerCameraOffset(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleLoadVisionPositions(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleLoadHandPitchOffsets(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleSaveTeachingPositions(VMF::Context& ctx, VMF::IActuator* actuator);

	public:
		CLoad1VATCommitTeachingPosTask();
		virtual ~CLoad1VATCommitTeachingPosTask();

        std::string GetName() const override
        {
            return "Task_CommitTeachingPos";
        }

	protected:
        void OnInitialize(VMF::Context& ctx) override;
        VMF::TaskResult OnPoll(VMF::Context& ctx, VMF::IActuator* actuator) override;

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
}
