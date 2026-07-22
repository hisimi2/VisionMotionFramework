#pragma once

#include "DefaultSetupStrategy.h"
#include "VMFEquipmentPluginExport.h"

// VMF 핵심 타입들이 정의된 헤더 포함 (전방 선언 대신 사용하여 재정의 방지)
#include "types.h"

namespace VMF_PLUGIN
{
    // 전방 선언만 필요한 클래스들
    class VisionPlviProcessor;
    class CSetPlate1PLVISequenceBuilder;
    class SqliteDataRepository;

    class VMF_PLUGIN_API SetPlate1PLVIStrategy : public VMF::DefaultSetupStrategy
    {
    private:
        // Helper: VisionParams에 값 설정
        void SetParam(VMF::VisionParams& params, const std::string& key, const std::string& value);
        void SetParam(VMF::VisionParams& params, const std::string& key, int value);
        void SetParam(VMF::VisionParams& params, const std::string& key, double value);

        // Helper: VisionPosition 추가
        void AddVisionPoint(VMF::VisionParams& params, int locateId, int requestId,
            double x, double y, double z);

        // Helper: VisionParams를 Repository에 저장
        void SaveVisionParamsToRepo(VMF::VisionContextPtr ctx,
            const VMF::VisionParams& params);

    public:
        std::string GetSequenceName() const override;
        VMF::DataRepositoryPtr CreateRepository() override;
        VMF::VisionProcessorPtr CreateVisionProcessor() override;
        VMF::SequenceBuilderPtr CreateBuilder() override;
        void ConfigureParams(VMF::VisionContextPtr ctx) override;
        VMF::StringMap GetVisionParams(const std::string& presetName) const override;
    };
}
