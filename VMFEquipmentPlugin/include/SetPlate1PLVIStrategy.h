#pragma once

#include "DefaultSetupStrategy.h"
#include "ParamKeys.h"
#include <string>
#include <memory>
#include <vector>

namespace VMF_PLUGIN
{
    class SetPlate1PLVIStrategy : public VMF::DefaultSetupStrategy
    {
    public:
        std::string GetSequenceName() const override;
        VMF::DataRepositoryPtr CreateRepository() override;
        VMF::VisionProcessorPtr CreateVisionProcessor() override;
        VMF::SequenceBuilderPtr CreateBuilder() override;
        void ConfigureContext(VMF::Context& ctx) override;
        VMF::TaskParams GetDefaultParams() const;
        VMF::TaskParams GetSetupParams() const;
        VMF::TaskParams GetExecuteScanParams() const;
        VMF::TaskParams GetFinishParams() const;
        VMF::TaskParams GetVisionParams() const;
        void SetTaskParamsByTask(VMF::Context& ctx) const;
        void SetTaskParamsForTask(VMF::Context& ctx, const std::string& taskName, const VMF::TaskParams& params) const;
    };
}

