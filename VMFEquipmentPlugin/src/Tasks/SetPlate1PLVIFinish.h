#pragma once
#include "NonBlockingTaskBase.h"
#include "IParamProvider.h"
#include "ParamKeys.h"

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
     * 
     * @note 사용하는 파라미터 (IParamProvider를 통해 조회):
     *   - timeoutMoveMs (int): 이동 타임아웃 [ms], 기본값 7000
     * 
     * @note 테스트 용이성:
     *   IParamProvider 인터페이스를 통해 파라미터를 조회하므로,
     *   단위 테스트 시 MockParamProvider를 주입하여 테스트할 수 있습니다.
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

        // ✅ Task별 파라미터 구조체 사용 (타입 안전성 및 가독성 향상)
        ParamKeys::FinishParams m_params;
    };
}
