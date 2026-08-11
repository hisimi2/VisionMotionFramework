#pragma once
#include "NonBlockingTaskBase.h"

namespace VMF_PLUGIN
{
    class CSetPlate1PLVIFinishTask : public VMF::NonBlockingTaskBase
    {
    public:
        CSetPlate1PLVIFinishTask();
        virtual ~CSetPlate1PLVIFinishTask();

        std::string GetName() const override
        {
            return "Task_PLVI_Finish";
        }

    protected:
        void OnInitialize(VMF::Context& ctx) override;
        VMF::TaskResult OnPoll(VMF::Context& ctx, VMF::IActuator* actuator) override;

    private:
        enum SubStep
        {
            MoveSafeZ = 0,
            WaitSafeZ,
            MoveHome,
            WaitHome,
            Complete
        };

        VMF::TaskResult HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleWaitSafeZ(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleMoveHome(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleWaitHome(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleComplete(VMF::Context& ctx, VMF::IActuator* actuator);

        long m_moveTimeoutMs;
    };
}
