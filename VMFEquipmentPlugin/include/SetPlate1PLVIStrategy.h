#pragma once

#include "DefaultSetupStrategy.h"
#include "VMFEquipmentPluginExport.h"
#include "types.h"

namespace VMF_PLUGIN
{
    // 전방 선언만 필요한 클래스들
    class VisionPlviProcessor;
    class SetPlate1PLVISequenceBuilder;
    class SqliteDataRepository;

    /**
     * @brief SetPlate1 PLVI 측정 전략
     * 
     * SetPlate1 기구의 PLVI(Pattern Learning Vision Inspection) 측정을 위한 전략을 정의합니다.
     * 
     * @details
     * 이 전략은 PLVI 측정에 필요한 파라미터 설정, 데이터베이스 관리, 시퀀스 빌더 생성을 담당합니다.
     * 
     * ┌─────────────────────────────────────────────────────────────────┐
     * │ 관련 클래스                                                     │
     * ├─────────────────────────────────────────────────────────────────┤
     * │ SetPlate1PLVISequenceBuilder  - PLVI 측정 시퀀스 빌더          │
     * │ SetPlate1PLVISetup              - 측정 준비 Task              │
     * │ SetPlate1PLVIExecuteScan        - 측정 실행 Task              │
     * │ SetPlate1PLVIFinish             - 측정 종료 Task              │
     * └─────────────────────────────────────────────────────────────────┘
     * 
     * @note 총 3개의 Task로 구성된 시퀀스를 생성합니다.
     */
    class VMF_PLUGIN_API SetPlate1PLVIStrategy : public VMF::DefaultSetupStrategy
    {
    public:
        std::string GetSequenceName() const override;
        VMF::DataRepositoryPtr CreateRepository() override;
        VMF::VisionProcessorPtr CreateVisionProcessor() override;
        VMF::SequenceBuilderPtr CreateBuilder() override;
        void ConfigureParams(VMF::VisionContextPtr ctx) override;
        VMF::StringMap GetVisionParams(const std::string& presetName) const override;
    };
}
