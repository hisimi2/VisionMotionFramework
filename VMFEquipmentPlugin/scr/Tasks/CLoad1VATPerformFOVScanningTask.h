#pragma once

#include "NonBlockingTaskBase.h"

namespace VMF_PLUGIN
{

	class CLoad1VATPerformFOVScanningTask : public VMF::NonBlockingTaskBase
    {
        enum Substep
        {
            MoveSafeZ = 0,
            MoveOrigin,
            MoveScanPosition,
            MoveFocusPositionZ,
            VisionRequest,
            VisionWait,
            ReturnHome,
        };

        VMF::TaskResult HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleMoveOrigin(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleMoveScanPosition(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleMoveFocusPositionZ(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleVisionRequest(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleVisionWait(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleReturnHome(VMF::Context& ctx, VMF::IActuator* actuator);

	public:
		CLoad1VATPerformFOVScanningTask();
		virtual ~CLoad1VATPerformFOVScanningTask();

        std::string GetName() const override
        {
            return "Task_PerformFOVScanning";
        }

		enum FOVDirection
		{
			CENTER = 0,
			REAR,
			FRONT,
			LEFT_SIDE,
			RIGHT_SIDE
		};

	protected:
        void OnInitialize(VMF::Context& ctx) override;
        VMF::TaskResult OnPoll(VMF::Context& ctx, VMF::IActuator* actuator) override;

		FOVDirection m_currentScanDirection;
		double m_centerPositionX;
		double m_centerPositionY;
		double m_focusPositionZ;
		int m_cameraId;
		double m_targetPositionX;
		double m_targetPositionY;
		double m_fovInterval;
		int m_visionTimeoutMs;
		long m_moveTimeoutMs;
	};
} 
