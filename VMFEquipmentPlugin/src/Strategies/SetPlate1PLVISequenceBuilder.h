#pragma once

#include "SequenceBuilderBase.h"
#include "VMFEquipmentPluginExport.h"
#include "Types.h"
#include "ParamKeys.h"

#include <memory>

namespace VMF_PLUGIN
{
    // 전방 선언
    class SetPlate1PLVIStrategy;

    class SetPlate1PLVISequenceBuilder : public VMF::SequenceBuilderBase
    {
    public:
        void ConfigureContext(VMF::Context& ctx) override;

    protected:
        VMF::SequencePtr BuildSequence(const std::string& sequenceName) override;

    public:
        // Strategy 참조 (Task별 파라미터 설정을 위해 필요)
        // SetPlate1PLVIStrategy::CreateBuilder()에서 설정됨
        std::shared_ptr<SetPlate1PLVIStrategy> m_strategy;
    };
}
