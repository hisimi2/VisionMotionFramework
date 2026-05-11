#pragma once

#include "NonBlockingTaskBase.h"

namespace VAT_LOAD1
{
	namespace Task
	{
		class CLoad1CommitPickerCamDistanceResultsTask : public VMF::NonBlockingTaskBase
		{
            // 파생 클래스는 범위화된 enum(class) 사용: 네임 충돌 제거 및 타입 안정성 확보
            enum Substep
            {
                CalcCamAlignPos = 0,
                GetStdPickerPos
            };

            // 단계별 핸들러
            VMF::TaskResult HandleCalcCamAlignPos(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);
            VMF::TaskResult HandleGetStdPickerPos(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator);

		public:
			CLoad1CommitPickerCamDistanceResultsTask();
			virtual ~CLoad1CommitPickerCamDistanceResultsTask();

            std::string GetName() const override
            {
                return "Task_CommitPickerCamDistanceResults";
            }

		protected:
            void OnInitialize(VMF::VAT_Context& ctx) override;
            VMF::TaskResult OnPoll(VMF::VAT_Context& ctx, VMF::IVatActuator* actuator) override;

			int m_lowerCameraId;
			int m_packageId;
			int m_pickerHandId;
			std::vector<int> m_upperCameraIds;
			size_t m_currentUpperCameraIndex;
			double m_pickerReferencePosX_Narrow;
			double m_pickerReferencePosY_Narrow;
			double m_pickerReferencePosX_Wide;
			double m_pickerReferencePosY_Wide;
            VMF::PitchStatus m_currentPitchMode;
		};
    } // namespace Task
} // namespace VAT_LOAD1 
