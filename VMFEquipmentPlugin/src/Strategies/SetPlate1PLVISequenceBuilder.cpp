#include "pch.h"
#include "SetPlate1PLVISequenceBuilder.h"
#include "Context.h"

#include "src\Tasks\SetPlate1PLVISetup.h"
#include "src\Tasks\SetPlate1PLVIExecuteScan.h"
#include "src\Tasks\SetPlate1PLVIFinish.h"

using namespace VMF;
using namespace VMF_PLUGIN;

void SetPlate1PLVISequenceBuilder::ConfigureContext(VMF::Context& ctx)
{
    // Context에 시퀀스 실행에 필요한 파라미터를 설정
    // 이 파라미터들은 Tasks가 Context를 통해 접근할 수 있음
    
    // ExecuteScan Task용 파라미터 설정
    VisionParams executeParams;
    executeParams.visionParams["SCAN_END_Y"] = "200.0";
    executeParams.visionParams["TIMEOUT_MOVE_MS"] = "7000";
    executeParams.visionParams["TIMEOUT_RESULT_MS"] = "10000";
    
    // VisionPositions 설정 (시작 위치)
    VisionPosition startPos;
    startPos.pos = {0.0, 0.0, 0.0};  // X=0, Y=0, Z=0
    startPos.locateId = 0;
    startPos.requestId = 1;
    executeParams.visionPositions.push_back(startPos);
    
    // Context에 파라미터 설정
    ctx.SetTaskParams(executeParams);
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
