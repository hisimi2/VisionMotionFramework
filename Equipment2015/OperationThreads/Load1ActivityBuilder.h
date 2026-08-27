#pragma once
#include "EquipmentCore/ActivityBuilderBase.h"

class Load1Parts;
namespace EC
{
    class Context;
    class CLoad1ActivityBuilder : public ActivityBuilderBase
    {
        std::shared_ptr<Load1Parts> m_parts;

    public:
        void SetParts(std::shared_ptr<Load1Parts> parts) { m_parts = parts; }

    protected:
        ActivityPtr Build() override;
        void ConfigureParams(ContextPtr ctx) override;
    };
}
