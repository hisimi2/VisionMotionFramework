#include "pch.h"
#include "SetPlate1PLVISequenceBuilder.h"
#include "SetPlate1PLVIStrategy.h"
#include "Context.h"

#include "src\Tasks\SetPlate1PLVISetup.h"
#include "src\Tasks\SetPlate1PLVIExecuteScan.h"
#include "src\Tasks\SetPlate1PLVIFinish.h"

#include <cassert>  // assert 사용

using namespace VMF;
using namespace VMF_PLUGIN;

void SetPlate1PLVISequenceBuilder::ConfigureContext(VMF::Context& ctx)
{
    // Strategy는 필수이므로 assert로 검증
    // Strategy가 설정되지 않은 경우 오류를 조기에 발견
    assert(m_strategy && "SetPlate1PLVISequenceBuilder: m_strategy must be set before ConfigureContext()");
    
    // ✅ Strategy의 SetTaskParamsByTask()를 호출하여 Task별 파라미터 설정
    // Strategy가 Task별 파라미터를 Context에 분리하여 저장
    m_strategy->SetTaskParamsByTask(ctx);
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
