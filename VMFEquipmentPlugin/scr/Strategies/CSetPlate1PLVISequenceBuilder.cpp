#include "pch.h"
#include "CSetPlate1PLVISequenceBuilder.h"

#include "scr\Tasks\CSetPlate1PLVIMoveToScanStartTask.h"
#include "scr\Tasks\CSetPlate1PLVIPerformScanTask.h"

using namespace VMF_PLUGIN;


VMF::SequencePtr CSetPlate1PLVISequenceBuilder::BuildSequence(
    const std::string& sequenceName) 
{
    VMF::SequencePtr seq(new VMF::Sequence(sequenceName));

    // ── MoveToScanStart ─────────────────────────────────
    {
        auto task = std::make_shared<CSetPlate1PLVIMoveToScanStartTask>();
        VMF::VisionParams mp;
        mp.visionParams["TIMEOUT_MOVE_MS"] = "7000";
        mp.visionParams["TRIGGER_INTERVAL_MM"] = "2.0";
        mp.visionParams["SCAN_SPEED_MM_S"] = "100.0";
        task->SetTaskParams(mp);
        seq->AddTask(task);
    }

    // ── PerformScan ─────────────────────────────────────
    {
        auto task = std::make_shared<CSetPlate1PLVIPerformScanTask>();
        VMF::VisionParams ip;
        ip.visionParams["PLVI_POSITION"] = "0";
        ip.visionParams["CTRAY_X"] = "8";
        ip.visionParams["CTRAY_Y"] = "4";
        ip.visionParams["DATA_ID"] = "1";
        ip.visionParams["SCAN_SPEED_MM_S"] = "100.0";
        ip.visionParams["MAX_RETRY_COUNT"] = "3";
        ip.visionParams["TIMEOUT_MEASURE_MS"] = "5000";
        ip.visionParams["TIMEOUT_SCAN_MS"] = "15000";
        ip.visionParams["TIMEOUT_RESULT_MS"] = "10000";
        ip.visionParams["TIMEOUT_MOVE_MS"] = "7000";
        task->SetTaskParams(ip);
        seq->AddTask(task);
    }

    return VMF::SequencePtr(seq.release());
}
