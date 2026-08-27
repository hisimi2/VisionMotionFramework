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
        VMF::TaskParams GetDefaultParams() const;
        VMF::TaskParams GetSetupParams() const;
        VMF::TaskParams GetExecuteScanParams() const;
        VMF::TaskParams GetFinishParams() const;
        VMF::TaskParams GetVisionParams() const;
        void SetTaskParamsByTask(VMF::Context& ctx) const;
        void SetTaskParamsForTask(VMF::Context& ctx, const std::string& taskName, const VMF::TaskParams& params) const;
        
    public:
        std::string GetName() const override;
        VMF::DataRepositoryPtr CreateRepository() override;
        VMF::VisionProcessorPtr CreateVision() override;
        VMF::SequenceBuilderPtr CreateBuilder() override;
        void ConfigureContext(VMF::Context& ctx) override;
        
        /// IComponentSetup 인터페이스 구현: preset별 Vision 파라미터 제공
        VMF::StringMap GetVisionParams(const std::string& presetName) const override;
    };
}

