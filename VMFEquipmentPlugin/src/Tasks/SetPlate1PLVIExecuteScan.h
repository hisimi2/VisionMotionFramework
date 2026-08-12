#pragma once
#include "NonBlockingTaskBase.h"

namespace VMF_PLUGIN
{
    /**
     * @brief SetPlate1 PLVI 측정 실행 Task
     * 
     * PLVI 측정을 실제로 실행하는 작업을 수행합니다.
     * 
     * @details
     * 이 Task는 4개의 SubStep으로 구성되어 측정 영역으로 이동하고 결과를 수신합니다.
     * 
     * ┌─────────────────────────────────────────────────────────────┐
     * │ SubStep                 │ 설명                             │
     * ├─────────────────────────┼──────────────────────────────────┤
     * │ MoveMeasurementArea     │ 측정 영역으로 이동               │
     * │ WaitMeasurementArea     │ 측정 영역 도착 대기              │
     * │ RequestResult           │ 측정 결과 요청                   │
     * │ WaitResult              │ 측정 결과 수신 대기              │
     * │ Complete                │ Task 완료                        │
     * └─────────────────────────┴──────────────────────────────────┘
     * 
     * @note 총 4개의 SubStep으로 구성됨
     */
    class SetPlate1PLVIExecuteScan : public VMF::NonBlockingTaskBase
    {
    public:
        SetPlate1PLVIExecuteScan();
        virtual ~SetPlate1PLVIExecuteScan();

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
