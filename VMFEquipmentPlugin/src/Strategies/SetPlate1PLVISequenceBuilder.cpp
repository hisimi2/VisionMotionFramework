#include "pch.h"
#include "SetPlate1PLVISequenceBuilder.h"
#include "Context.h"

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
    // Strategy에서 설정한 기본 파라미터(m_taskParams)를 Context에 전달
    // m_taskParams는 SetTaskParams()를 통해 Strategy::GetDefaultTaskParams()에서 설정됨
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
