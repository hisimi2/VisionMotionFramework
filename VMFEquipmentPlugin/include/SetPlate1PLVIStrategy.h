// D:\01GitHub_PROJECT\VisionMotionFramework\VMFEquipmentPlugin\include\SetPlate1PLVIStrategy.h
#pragma once
#include "PlugInAPI.h"
#include "ParamKeys.h"
#include <string>
#include <memory>
#include <vector>

#include "VisionMotionFramework/DefaultSetupStrategy.h"

namespace VMF_PLUGIN
{
    class VMF_PLUGIN_API SetPlate1PLVIStrategy : public VMF::DefaultSetupStrategy
    {
    public:
        std::string GetName() const override;
        VMF::DataRepositoryPtr CreateRepository() override;
        VMF::VisionProcessorPtr CreateVision() override;
        VMF::SequenceBuilderPtr CreateBuilder() override;
        void ConfigureContext(VMF::Context& ctx) override;

        /// IComponentSetup interface implementation: provide Vision params by preset
        VMF::StringMap GetVisionParams(const std::string& presetName) const override;

    protected:
        // ── 파라미터 관련 함수 오버라이드 (DefaultSetupStrategy에서 일반화) ──
        VMF::TaskParams GetDefaultParams() const override;
        VMF::TaskParams GetVisionParams() const override;
        void SetTaskParamsByTask(VMF::Context& ctx) const override;

        // -- Task별 파라미터 제공 함수 (Strategy에서 직접 구현) --
        VMF::TaskParams GetSetupParams() const;
        VMF::TaskParams GetExecuteScanParams() const;
        VMF::TaskParams GetFinishParams() const;
    };
}
