#pragma once
#include "NonBlockingTaskBase.h"

namespace VMF_PLUGIN
{
	class CSetPlate1PLVIMoveToScanStartTask : public VMF::NonBlockingTaskBase
	{
		enum Substep
		{
			MoveSafeZ = 0,
			WaitSafeZ,
			MoveScanStartXY,
			WaitScanStartXY,
			MoveVisionZ,
			WaitVisionZ,
			SetupTrigger,
			Complete,
		};

		VMF::TaskResult HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleWaitSafeZ(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleMoveScanStartXY(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleWaitScanStartXY(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleMoveVisionZ(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleWaitVisionZ(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleSetupTrigger(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleComplete(VMF::Context& ctx, VMF::IActuator* actuator);

	public:
		CSetPlate1PLVIMoveToScanStartTask();
		virtual ~CSetPlate1PLVIMoveToScanStartTask();

		std::string GetName() const override
		{
			return "Task_PLVI_MoveToScanStart";
		}

	protected:
		void            OnInitialize(VMF::Context& ctx) override;
		VMF::TaskResult OnPoll(VMF::Context& ctx,
			VMF::IActuator* actuator) override;

		std::vector<double> m_scanStartPos;      // [0]=X [1]=Y [2]=VisionZ
		double              m_triggerIntervalMm;
		double              m_scanSpeedMmS;
		long                m_moveTimeoutMs;


        VMF::VisionPosition m_measurepos;
	};
}
