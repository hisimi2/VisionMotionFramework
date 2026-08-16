#include "pch.h"
#include "SetPlate1PLVISequenceBuilder.h"
#include "SetPlate1PLVIStrategy.h"
#include "Context.h"

#include "src\Tasks\SetPlate1PLVISetup.h"
#include "src\Tasks\SetPlate1PLVIExecuteScan.h"
#include "src\Tasks\SetPlate1PLVIFinish.h"

using namespace VMF;
using namespace VMF_PLUGIN;

void SetPlate1PLVISequenceBuilder::ConfigureContext(VMF::Context& ctx)
{
    if (m_strategy)
    {
        // ✅ Strategy의 SetTaskParamsByTask()를 호출하여 Task별 파라미터 설정
        // Strategy가 Task별 파라미터를 Context에 분리하여 저장
        m_strategy->SetTaskParamsByTask(ctx);
    }
    else
    {
        // Strategy가 없는 경우 기본 파라미터 사용 (하위 호환성)
        // SetPlate1PLVIStrategy::GetDefaultTaskParams()는 Strategy가 있을 때만 접근 가능
        // Strategy가 없으면 빈 TaskParams 사용
        ctx.SetTaskParams(VMF::TaskParams());
    }
}

VMF::SequencePtr SetPlate1PLVISequenceBuilder::BuildSequence(const std::string& sequenceName)
{
    VMF::SequencePtr seq(new VMF::Sequence(sequenceName));

    // 1. Setup Task
    {
        auto task = std::make_shared<SetPlate1PLVISetup>();
        seq->AddTask(task);
    }

    // 2. Execute Scan Task
    {
        auto task = std::make_shared<SetPlate1PLVIExecuteScan>();
        seq->AddTask(task);
    }

    // 3. Finish Task
    {
        auto task = std::make_shared<SetPlate1PLVIFinish>();
        seq->AddTask(task);
    }

    return seq;
}
