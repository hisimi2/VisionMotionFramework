#include "pch.h"
#include "SetPlate1PLVISequenceBuilder.h"
#include "Context.h"

#include "src\Tasks\SetPlate1PLVISetup.h"
#include "src\Tasks\SetPlate1PLVIExecuteScan.h"
#include "src\Tasks\SetPlate1PLVIFinish.h"

using namespace VMF;
using namespace VMF_PLUGIN;

// Helper: VisionParams에 string 값 설정
void SetPlate1PLVISequenceBuilder::SetParam(VisionParams& params, const std::string& key, const std::string& value)
{
    params.visionParams[key] = value;
}

// Helper: VisionParams에 int 값 설정
void SetPlate1PLVISequenceBuilder::SetParam(VisionParams& params, const std::string& key, int value)
{
    params.visionParams[key] = std::to_string(value);
}

// Helper: VisionParams에 double 값 설정
void SetPlate1PLVISequenceBuilder::SetParam(VisionParams& params, const std::string& key, double value)
{
    std::ostringstream oss;
    oss << value;
    params.visionParams[key] = oss.str();
}

void SetPlate1PLVISequenceBuilder::ConfigureContext(VMF::Context& ctx)
{
    // Context에 시퀀스 실행에 필요한 파라미터를 설정
    // 이 파라미터들은 Tasks가 Context를 통해 접근할 수 있음
    
    // ExecuteScan Task용 파라미터 설정
    VisionParams executeParams;
    executeParams.visionParams["SCAN_END_Y"] = "200.0";
    executeParams.visionParams["TIMEOUT_MOVE_MS"] = "7000";
    executeParams.visionParams["TIMEOUT_RESULT_MS"] = "10000";
    
    // Context에 파라미터 설정
    ctx.SetTaskParams(executeParams);
}

VMF::SequencePtr SetPlate1PLVISequenceBuilder::BuildSequence(const std::string& sequenceName)
{
    VMF::SequencePtr seq(new VMF::Sequence(sequenceName));

    // 1. Setup Task
    {
        auto task = std::make_shared<SetPlate1PLVISetup>();
        VMF::VisionParams mp;
        mp.visionParams["TIMEOUT_MOVE_MS"] = "7000";
        mp.visionParams["TRIGGER_INTERVAL_MM"] = "1.8";
        task->SetTaskParams(mp);
        seq->AddTask(task);
    }

    // 2. Execute Scan Task
    {
        auto task = std::make_shared<SetPlate1PLVIExecuteScan>();
        VMF::VisionParams ip;
        ip.visionParams["SCAN_END_Y"] = "200.0";
        ip.visionParams["TIMEOUT_MOVE_MS"] = "7000";
        ip.visionParams["TIMEOUT_RESULT_MS"] = "10000";
        task->SetTaskParams(ip);
        seq->AddTask(task);
    }

    // 3. Finish Task
    {
        auto task = std::make_shared<SetPlate1PLVIFinish>();
        VMF::VisionParams fp;
        fp.visionParams["TIMEOUT_MOVE_MS"] = "7000";
        task->SetTaskParams(fp);
        seq->AddTask(task);
    }

    return seq;
}
