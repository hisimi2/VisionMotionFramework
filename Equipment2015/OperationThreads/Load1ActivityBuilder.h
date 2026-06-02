#pragma once
#include "ActivityBuilderBase.h"
#include "Load1TaskPick.h"
#include "Load1TaskPlace.h"

namespace OperationThread
{
    using namespace EC;

    /// <summary>
    /// Load1 Activity Builder
    /// Load1TaskPick + Load1TaskPlace로 Activity 구성
    /// 
    /// Load1은 XY축 + Setplate/Transfer 실린더를 사용합니다.
    /// </summary>
    class CLoad1ActivityBuilder : public ActivityBuilderBase
    {
        std::shared_ptr<Load1Parts> m_parts;

    protected:
        ActivityPtr Build() override
        {
            m_parts = std::make_shared<Load1Parts>();

            auto activity = std::make_unique<Activity>("Load1Activity");
            activity->AddTask(std::make_shared<Load1TaskPick>(m_parts));
            activity->AddTask(std::make_shared<Load1TaskPlace>(m_parts));

            return activity;
        }

        void ConfigureParams(ContextPtr ctx) override
        {
            ctx->SetParamAs<int>("repeatCount", 2);
            ctx->SetParamAs<int>("timeout", 3000);

            // Load1 기본 위치 파라미터
            ctx->SetParamAs<double>("pickX", 100.0);
            ctx->SetParamAs<double>("pickY", 200.0);
            ctx->SetParamAs<double>("pickZ", -10.0);
            ctx->SetParamAs<double>("placeX", 300.0);
            ctx->SetParamAs<double>("placeY", 150.0);
            ctx->SetParamAs<double>("placeZ", -12.0);
            ctx->SetParamAs<double>("safeZ", 0.0);
            ctx->SetParamAs<int>("clampIndex", 0);
            ctx->SetParamAs<int>("vacuumIndex", 0);
        }
    };
}
