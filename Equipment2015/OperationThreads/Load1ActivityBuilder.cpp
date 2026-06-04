#include "stdafx.h"
#include "Load1ActivityBuilder.h"

#include "Activity.h"
#include "../../EquipmentCore/include/Context.h"
#include "Load1TaskPick.h"
#include "Load1TaskPlace.h"

namespace OperationThread
{
    ActivityPtr CLoad1ActivityBuilder::Build()
    {
        m_parts = std::make_shared<Load1Parts>();

        auto activity = std::make_unique<Activity>("Load1Activity");
        activity->AddTask(std::make_shared<Load1TaskPick>(m_parts));
        activity->AddTask(std::make_shared<Load1TaskPlace>(m_parts));

        return activity;
    }

    void CLoad1ActivityBuilder::ConfigureParams(ContextPtr ctx)
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
}
