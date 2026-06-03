#include "stdafx.h"
#include "Load2ActivityBuilder.h"
#include "Activity.h"
#include "../../EquipmentCore/include/Context.h"

namespace OperationThread
{
    ActivityPtr CLoad2ActivityBuilder::Build()
    {
        m_parts = std::make_shared<Load2Parts>();

        auto activity = std::make_unique<Activity>("Load2Activity");
        activity->AddTask(std::make_shared<Load2TaskPick>(m_parts));
        activity->AddTask(std::make_shared<Load2TaskPlace>(m_parts));

        return activity;
    }

    void CLoad2ActivityBuilder::ConfigureParams(ContextPtr ctx)
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
}