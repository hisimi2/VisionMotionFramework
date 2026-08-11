#pragma once
#include "NonBlockingTaskBase.h"

namespace VMF_PLUGIN
{
    class CSetPlate1PLVIExecuteScanTask : public VMF::NonBlockingTaskBase
    {
    public:
        CSetPlate1PLVIExecuteScanTask();
        virtual ~CSetPlate1PLVIExecuteScanTask();

        std::string GetName() const override
        {
            return "Task_PLVI_ExecuteScan";
        }

    protected:
        void OnInitialize(VMF::Context& ctx) override;
        VMF::TaskResult OnPoll(VMF::Context& ctx, VMF::IActuator* actuator) override;

    private:
        enum SubStep
        {
            MoveMeasurementArea = 0,
            WaitMeasurementArea,
            RequestResult,
            WaitResult,
            Complete
        };

        VMF::TaskResult HandleMoveMeasurementArea(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleWaitMeasurementArea(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleRequestResult(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleWaitResult(VMF::Context& ctx, VMF::IActuator* actuator);
        VMF::TaskResult HandleComplete(VMF::Context& ctx, VMF::IActuator* actuator);

        double              m_scanEndY;
        long                m_timeoutMoveMs;
        long                m_timeoutResultMs;
    };
}
