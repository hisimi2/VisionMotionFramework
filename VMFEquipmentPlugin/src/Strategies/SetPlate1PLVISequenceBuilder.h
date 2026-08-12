#pragma once
#include "SequenceBuilderBase.h"
#include "VMFEquipmentPluginExport.h"


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
        /// Context에 시퀀스 실행에 필요한 파라미터를 설정합니다.
        /// 이 메서드는 시퀀스 실행 전에 호출되어 Context에 파라미터를 저장합니다.
        /// </summary>
        void ConfigureContext(VMF::Context& ctx) override;

protected:
        VMF::SequencePtr BuildSequence(
            const std::string& sequenceName) override;
    };
} // namespace VMF_PLUGIN
