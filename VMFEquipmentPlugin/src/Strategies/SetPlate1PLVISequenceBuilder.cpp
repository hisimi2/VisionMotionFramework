#include "pch.h"
#include "SetPlate1PLVISequenceBuilder.h"
#include "Context.h"
#include "SetPlate1PLVIParams.h"

#include "src\Tasks\SetPlate1PLVISetup.h"
#include "src\Tasks\SetPlate1PLVIExecuteScan.h"
#include "src\Tasks\SetPlate1PLVIFinish.h"

using namespace VMF;
using namespace VMF_PLUGIN;

void SetPlate1PLVISequenceBuilder::SetTaskParams(const VMF::TaskParams& params)
{
    m_taskParams = params;
}

void SetPlate1PLVISequenceBuilder::ConfigureContext(VMF::Context& ctx)
{
    // 미리 설정된 Task 파라미터를 Context에 적용
    // 파라미터는 SetTaskParams()로 설정되며, 일반적으로 Strategy::GetDefaultTaskParams()에서 얻습니다.
    
    ctx.SetTaskParams(m_taskParams);
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
