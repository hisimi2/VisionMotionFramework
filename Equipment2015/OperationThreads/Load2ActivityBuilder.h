#pragma once

#include "EquipmentCore/ActivityBuilderBase.h"

class Load2Parts;

namespace EC
{
    class Context;

    class CLoad2ActivityBuilder : public ActivityBuilderBase
    {
        std::shared_ptr<Load2Parts> m_parts;

    public:
        void SetParts(std::shared_ptr<Load2Parts> parts) { m_parts = parts; }

    protected:
        ActivityPtr Build() override;
        void ConfigureParams(ContextPtr ctx) override;
    };
}
