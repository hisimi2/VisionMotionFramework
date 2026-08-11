#include "pch.h"
#include "CSetPlate1PLVISequenceBuilder.h"

#include "src\Tasks\CSetPlate1PLVISetupTask.h"
#include "src\Tasks\CSetPlate1PLVIExecuteScanTask.h"
#include "src\Tasks\CSetPlate1PLVIFinishTask.h"

using namespace VMF;
using namespace VMF_PLUGIN;

VMF::SequencePtr CSetPlate1PLVISequenceBuilder::BuildSequence(const std::string& sequenceName)
{
    VMF::SequencePtr seq(new VMF::Sequence(sequenceName));

    // 1. Setup Task
    {
        auto task = std::make_shared<CSetPlate1PLVISetupTask>();
        VMF::VisionParams mp;
        mp.visionParams["TIMEOUT_MOVE_MS"] = "7000";
        mp.visionParams["TRIGGER_INTERVAL_MM"] = "1.8";
        task->SetTaskParams(mp);
        seq->AddTask(task);
    }

    // 2. Execute Scan Task
    {
        auto task = std::make_shared<CSetPlate1PLVIExecuteScanTask>();
        VMF::VisionParams ip;
        ip.visionParams["SCAN_END_Y"] = "200.0";
        ip.visionParams["TIMEOUT_MOVE_MS"] = "7000";
        ip.visionParams["TIMEOUT_RESULT_MS"] = "10000";
        task->SetTaskParams(ip);
        seq->AddTask(task);
    }

    // 3. Finish Task
    {
        auto task = std::make_shared<CSetPlate1PLVIFinishTask>();
        VMF::VisionParams fp;
        fp.visionParams["TIMEOUT_MOVE_MS"] = "7000";
        task->SetTaskParams(fp);
        seq->AddTask(task);
    }

    return seq;
}
