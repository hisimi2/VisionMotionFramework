#pragma once
#include "NonBlockingTaskBase.h"

namespace VMF_PLUGIN
{
    class CSetPlate1PLVISetupTask : public VMF::NonBlockingTaskBase
    {
    public:
        CSetPlate1PLVISetupTask();
        virtual ~CSetPlate1PLVISetupTask();

        std::string GetName() const override
        {
            return "Task_PLVI_Setup";
        }

    protected:
        void OnInitialize(VMF::Context& ctx) override;
        VMF::TaskResult OnPoll(VMF::Context& ctx, VMF::IActuator* actuator) override;

    private:
        enum SubStep
        {
            MoveSafeZ = 0,
            WaitSafeZ,
            MoveHorizontalStart,
            WaitHorizontalStart,
            SetupTrigger,
            Complete
        };

        VMF::TaskResult HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleWaitSafeZ(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleMoveHorizontalStart(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleWaitHorizontalStart(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleSetupTrigger(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleComplete(VMF::Context& ctx, VMF::IActuator* actuator);

        double              m_scanStartX;
        double              m_scanStartY;
        double              m_scanStartZ;
        double              m_triggerIntervalMm;
        long                m_moveTimeoutMs;
        VMF::VisionPosition m_measurepos;
    };
}
