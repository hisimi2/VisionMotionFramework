#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1CommitTeachingPosTask : public DVH_VAT::NonBlockingTaskBase
		{
            enum Substep
            {
                LoadPickerCameraOffset = 0,
                LoadVisionPositions,
                LoadHandPitchOffsets,
                SaveTeachingPositions
            };

            DVH_VAT::TaskResult HandleLoadPickerCameraOffset(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleLoadVisionPositions(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleLoadHandPitchOffsets(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);
            DVH_VAT::TaskResult HandleSaveTeachingPositions(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator);

		public:
			CLoad1CommitTeachingPosTask();
			virtual ~CLoad1CommitTeachingPosTask();

            std::string GetName() const override
            {
                return "Task_CommitTeachingPos";
            }

		protected:
            void OnInitialize(DVH_VAT::VAT_Context& ctx) override;
            DVH_VAT::TaskResult OnPoll(DVH_VAT::VAT_Context& ctx, DVH_VAT::IVatActuator* actuator) override;

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
			std::vector<DVH_VAT::VisionPosition> m_teachingPositions;
			std::vector<HandPitchOffset> m_handPitchOffsets;
		};
    } // namespace Task
} // namespace VAT_LOAD1 
