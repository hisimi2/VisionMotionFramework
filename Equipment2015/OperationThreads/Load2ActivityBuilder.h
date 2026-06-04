#pragma once
#include "ActivityBuilderBase.h"
#include "Actuators\Load2Parts.h"

namespace OperationThread
{
    using namespace EC;

    /// <summary>
    /// Load2 Activity Builder
    /// Load2TaskPick + Load2TaskPlace로 Activity 구성
    /// 
    /// Load2는 Load1과 달리 Y축이 없고, XPitch를 사용합니다.
    /// </summary>
    class CLoad2ActivityBuilder : public ActivityBuilderBase
    {
        std::shared_ptr<Load2Parts> m_parts;

    protected:
        ActivityPtr Build() override;
        void ConfigureParams(ContextPtr ctx) override;
    };
}
