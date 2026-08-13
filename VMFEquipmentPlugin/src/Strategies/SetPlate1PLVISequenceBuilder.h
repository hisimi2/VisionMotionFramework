#pragma once
#include "SequenceBuilderBase.h"
#include "VMFEquipmentPluginExport.h"
#include "Types.h"  // TaskParams 사용을 위해 추가

namespace VMF_PLUGIN
{
    class Context;

    /**
     * @brief SetPlate1 PLVI 측정 시퀀스 빌더
     * 
     * SetPlate1 기구의 PLVI(Pattern Learning Vision Inspection) 측정 시퀀스를 생성합니다.
     * 
     * @details
     * 이 빌더는 3개의 Task를 순차적으로 실행하여 PLVI 측정을 수행합니다.
     * 
     * @note 책임 분리:
     *   - Strategy(SetPlate1PLVIStrategy): PLVI 측정용 기본 파라미터 정의 (GetDefaultTaskParams)
     *   - Builder(SetPlate1PLVISequenceBuilder): Strategy로부터 받은 파라미터를 Context에 적용
     *   - Task: Context에서 파라미터를 조회하여 실행
     * 
     * ┌─────────────────────────────────────────────────────────────────┐
     * │ Task 클래스별 상세 정보                                           │
     * ├─────────────────────────────┬───────────┬─────────────────────┤
     * │ Task 클래스                   │ SubStep 수│ 담당 업무           │
     * ├─────────────────────────────┼───────────┼─────────────────────┤
     * │ SetPlate1PLVISetup           │ 5개       │ Z안전 이동 → 수평  │
     * │                             │           │ 시작 이동 → 트리거  │
     * │                             │           │ 설정                │
     * ├─────────────────────────────┼───────────┼─────────────────────┤
     * │ SetPlate1PLVIExecuteScan     │ 4개       │ 측정 영역 이동 →   │
     * │                             │           │ 결과 요청/수신      │
     * ├─────────────────────────────┼───────────┼─────────────────────┤
     * │ SetPlate1PLVIFinish        │ 4개       │ Z안전 이동 → 대기  │
     * │                             │           │ 위치 이동           │
     * └─────────────────────────────┴───────────┴─────────────────────┘
     * 
     * @note 총 13개의 SubStep으로 구성된 시퀀스를 생성합니다.
     */
    class VMF_PLUGIN_API SetPlate1PLVISequenceBuilder : public VMF::SequenceBuilderBase
    {
    public:
        SetPlate1PLVISequenceBuilder() = default;
        ~SetPlate1PLVISequenceBuilder() override = default;

        /// <summary>
        /// Builder가 사용할 Task 파라미터를 설정합니다.
        /// </summary>
        /// <details>
        /// 이 메서드는 Strategy(SetPlate1PLVIStrategy)로부터 받은 기본 파라미터를
        /// Builder에 저장합니다. ConfigureContext() 호출 시 이 저장된 파라미터를
        /// Context에 적용합니다.
        /// 
        /// @note 일반적으로 Strategy::GetDefaultTaskParams()로 얻은 파라미터를 전달합니다.
        /// </details>
        /// <param name="params">적용할 Task 파라미터</param>
        void SetTaskParams(const VMF::TaskParams& params);

        /// <summary>
        /// Context에 시퀀스 실행에 필요한 파라미터를 설정합니다.
        /// 이 메서드는 시퀀스 실행 전에 호출되어 Context에 실행 파라미터를 저장합니다.
        /// </summary>
        /// <details>
        /// 이 메서드는 SetTaskParams()로 미리 설정된 파라미터를 Context에 적용합니다.
        /// Task들은 OnInitialize에서 ctx.GetTaskParamAs<T>()를 통해 이 파라미터들을 조회합니다.
        /// 
        /// 주요 설정 파라미터:
        /// - Setup Task: timeoutMoveMs, triggerIntervalMm, startPos
        /// - ExecuteScan Task: timeoutMoveMs, timeoutResultMs, scanEndY
        /// - Finish Task: timeoutMoveMs
        /// - 공통 Vision 파라미터: HAND_ID, PKG_ID, DEVICE_INFO_* 등
        /// - VisionPositions: 시작 위치 정보
        /// 
        /// 전략 레벨의 파라미터(Repository 저장 필요)는 
        /// SetPlate1PLVIStrategy::ConfigureParams()에서 처리해야 합니다.
        /// </details>
        void ConfigureContext(VMF::Context& ctx) override;

protected:
        VMF::SequencePtr BuildSequence(
            const std::string& sequenceName) override;

private:
        VMF::TaskParams m_taskParams;  // Strategy로부터 받은 파라미터 저장
    };
} // namespace VMF_PLUGIN
