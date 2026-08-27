#pragma once
#include "VisionMotionFramework\NonBlockingTaskBase.h"
#include "VisionMotionFramework\IParamProvider.h"
#include "ParamKeys.h"

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
     * ┌─────────────────────────────────────────────────────────┐
     * │ SubStep                 │ 설명                            │
     * ├─────────────────────────┼───────────────────────────────
     * │ MoveMeasurementArea     │ 측정 영역으로 이동              │
     * │ WaitMeasurementArea     │ 측정 영역 도착 대기             │
     * │ RequestResult           │ 측정 결과 요청                 │
     * │ WaitResult              │ 측정 결과 수신 대기             │
     * │ Complete                │ Task 완료                     │
     * └─────────────────────────┴───────────────────────────────
     * 
     * @note 총 4개의 SubStep으로 구성됨
     * 
     * @note 사용하는 파라미터 (IParamProvider를 통해 조회):
     *   - timeoutMoveMs (int): 이동 타임아웃 [ms], 기본값 7000
     *   - timeoutResultMs (int): 결과 수신 타임아웃 [ms], 기본값 10000
     *   - visionPositions[1]: 측정 종료 위치 (VisionPosition, visionRequestId=3)
     * 
     * @note 테스트 용이성:
     *   IParamProvider 인터페이스를 통해 파라미터를 조회하므로,
     *   단위 테스트 시 MockParamProvider를 주입하여 테스트할 수 있습니다.
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

        // ✅ Task 파라미터
        int m_timeoutMoveMs;
        int m_timeoutResultMs;
        
        // ✅ VisionPosition 기반 측정 종료 위치 (scanEndY 대체)
        VMF::VisionPosition m_scanEndPos;
    };
}
