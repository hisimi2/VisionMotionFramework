#pragma once
#include "ActivityBuilderBase.h"
#include "Load2TaskPick.h"
#include "Load2TaskPlace.h"

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
        ActivityPtr Build() override
        {
            m_parts = std::make_shared<Load2Parts>();

            auto activity = std::make_unique<Activity>("Load2Activity");
            activity->AddTask(std::make_shared<Load2TaskPick>(m_parts));
            activity->AddTask(std::make_shared<Load2TaskPlace>(m_parts));

            return activity;
        }

        void ConfigureParams(ContextPtr ctx) override
        {
            ctx->SetParamAs<int>("repeatCount", 2);
            ctx->SetParamAs<int>("timeout", 3000);

            // Load2 기본 위치 파라미터
            ctx->SetParamAs<double>("pickX", 100.0);
            ctx->SetParamAs<double>("pickZ", -10.0);
            ctx->SetParamAs<double>("placeX", 200.0);
            ctx->SetParamAs<double>("placeZ", -12.0);
            ctx->SetParamAs<double>("safeZ", 0.0);
            ctx->SetParamAs<double>("xPitchWidth", 50.0);
            ctx->SetParamAs<int>("vacuumIndex", 0);
        }
    };
}