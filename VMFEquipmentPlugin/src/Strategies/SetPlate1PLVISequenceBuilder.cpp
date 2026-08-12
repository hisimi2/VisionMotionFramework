#include "pch.h"
#include "SetPlate1PLVISequenceBuilder.h"

#include "src\Tasks\SetPlate1PLVISetup.h"
#include "src\Tasks\SetPlate1PLVIExecuteScan.h"
#include "src\Tasks\SetPlate1PLVIFinish.h"

using namespace VMF;
using namespace VMF_PLUGIN;

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
