#pragma once
#include "NonBlockingTaskBase.h"

namespace VMF_PLUGIN
{
    /**
     * @brief SetPlate1 PLVI 측정 종료 Task
     * 
     * PLVI 측정 완료 후 정리 작업을 수행합니다.
     * 
     * @details
     * 이 Task는 4개의 SubStep으로 구성되어 Z축 안전 위치로 이동한 후 대기 위치로 이동합니다.
     * 
     * ┌─────────────────────────────────────────────────────────────┐
     * │ SubStep                 │ 설명                             │
     * ├─────────────────────────┼──────────────────────────────────┤
     * │ MoveSafeZ               │ Z축 안전 위치로 이동             │
     * │ WaitSafeZ               │ Z축 안전 위치 도착 대기          │
     * │ MoveHome                │ 대기(홈) 위치로 이동             │
     * │ WaitHome                │ 대기 위치 도착 대기              │
     * │ Complete                │ Task 완료                        │
     * └─────────────────────────┴──────────────────────────────────┘
     * 
     * @note 총 4개의 SubStep으로 구성됨
     */
    class SetPlate1PLVIFinish : public VMF::NonBlockingTaskBase
    {
    public:
        SetPlate1PLVIFinish();
        virtual ~SetPlate1PLVIFinish();

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
