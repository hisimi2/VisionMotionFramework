#pragma once
#include "NonBlockingTaskBase.h"

namespace VMF_PLUGIN
{
    /**
     * @brief SetPlate1 PLVI 측정 준비 Task
     * 
     * PLVI 측정을 시작하기 전의 준비 작업을 수행합니다.
     * 
     * @details
     * 이 Task는 5개의 SubStep으로 구성되어 측정 준비를 완료합니다.
     * 
     * ┌─────────────────────────────────────────────────────────────┐
     * │ SubStep                 │ 설명                               │
     * ├─────────────────────────┼──────────────────────────────────┤
     * │ MoveSafeZ               │ Z축 안전 위치로 이동             │
     * │ WaitSafeZ               │ Z축 안전 위치 도착 대기          │
     * │ MoveHorizontalStart     │ 수평 시작 위치로 이동            │
     * │ WaitHorizontalStart     │ 수평 시작 위치 도착 대기         │
     * │ SetupTrigger            │ 트리거 신호 및 레이저 설정       │
     * │ Complete                │ Task 완료                        │
     * └─────────────────────────┴──────────────────────────────────┘
     * 
     * @note 총 5개의 SubStep으로 구성됨
     */
    class SetPlate1PLVISetup : public VMF::NonBlockingTaskBase
    {
    public:
        SetPlate1PLVISetup();
        virtual ~SetPlate1PLVISetup();

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
    };
}
