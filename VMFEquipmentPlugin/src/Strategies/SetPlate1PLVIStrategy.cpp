// D:\01GitHub_PROJECT\VisionMotionFramework\VMFEquipmentPlugin\src\Strategies\SetPlate1PLVIStrategy.cpp
#include "pch.h"
#include "SetPlate1PLVIStrategy.h"

#include "VisionMotionFramework\SqliteDataRepository.h"
#include "..\Protocol\VisionPlviProcessor.h"
#include "..\Strategies\SetPlate1PLVISequenceBuilder.h"
#include "PLVITaskParamKeys.h"
#include "src\Protocol\VisionParamKeys.h"
#include <sstream>
#include <vector>

namespace VMF_PLUGIN
{
    /* 1. Sequence name – identifies the measurement sequence */
    std::string SetPlate1PLVIStrategy::GetName() const
    {
        return "SetPlate1PLVI";
    }

    /* 2. Repository creation – persists measurement data */
    VMF::DataRepositoryPtr SetPlate1PLVIStrategy::CreateRepository()
    {
        auto repo = std::make_shared<VMF::SqliteDataRepository>(
            "Data\\PLVI_DATABASE.db", "Data\\Images");
        repo->Initialize();
        return repo;
    }

    /* 3. Vision processor creation – starts the vision engine */
    VMF::VisionProcessorPtr SetPlate1PLVIStrategy::CreateVision()
    {
        VMF::VisionConnectionConfig config("127.0.0.1", 8000, 3000);
        auto vm = std::make_shared<VisionPlviProcessor>();
        vm->Initialize(config);
        return vm;
    }

    /* 4. Builder creation – only assembles Tasks (no parameter logic) */
    VMF::SequenceBuilderPtr SetPlate1PLVIStrategy::CreateBuilder()
    {
        // Builder only assembles Tasks; parameter configuration is done directly
        return std::make_shared<VMF_PLUGIN::SetPlate1PLVISequenceBuilder>();
    }

    /* 5. Parameter configuration – Strategy directly injects parameters
     *    into the Context (no Builder needed). */
    void SetPlate1PLVIStrategy::ConfigureContext(VMF::Context& ctx)
    {
        // Task‑specific parameter sets
        ctx.SetTaskParams("Task_PLVI_Setup", GetSetupParams());
        ctx.SetTaskParams("Task_PLVI_ExecuteScan", GetExecuteScanParams());
        ctx.SetTaskParams("Task_PLVI_Finish", GetFinishParams());
    }

    // ─────────────────────────────────────────────────────────────────────────
    // 파라미터 관련 함수 오버라이드 (DefaultSetupStrategy에서 일반화)
    // ─────────────────────────────────────────────────────────────────────────

    /* Setup Task – start position and trigger interval */
    VMF::TaskParams SetPlate1PLVIStrategy::GetSetupParams() const
    {
        VMF::TaskParams params;

        // Execution parameters
        params.SetExecutionParam(PLVI::Setup::TIMEOUT_MOVE_MS, 7000);
        params.SetExecutionParam(PLVI::Setup::TRIGGER_INTERVAL_MM, 2.0);

        // Vision position for start location
        VMF::VisionPosition startPos;
        startPos.pos = { 0.0, 0.0, 0.0 };
        startPos.locateId = 0;
        startPos.visionRequestId = 1;
        params.visionPositions.push_back(startPos);

        return params;
    }

    /* ExecuteScan Task – measurement start/end positions */
    VMF::TaskParams SetPlate1PLVIStrategy::GetExecuteScanParams() const
    {
        VMF::TaskParams params;

        // Execution parameters
        params.SetExecutionParam(PLVI::ExecuteScan::TIMEOUT_MOVE_MS, 7000);
        params.SetExecutionParam(PLVI::ExecuteScan::TIMEOUT_RESULT_MS, 10000);

        // Vision positions: start and end locations
        VMF::VisionPosition startPos;
        startPos.pos = { 0.0, 0.0, 0.0 };
        startPos.locateId = 0;
        startPos.visionRequestId = 2;               // start position
        params.visionPositions.push_back(startPos);

        VMF::VisionPosition scanEndPos;
        scanEndPos.pos = { 0.0, 200.0, 0.0 };
        scanEndPos.locateId = 0;
        scanEndPos.visionRequestId = 3;             // end position
        params.visionPositions.push_back(scanEndPos);

        return params;
    }

    /* Finish Task – safe‑Z and home positions */
    VMF::TaskParams SetPlate1PLVIStrategy::GetFinishParams() const
    {
        VMF::TaskParams params;

        // Execution timeout
        params.SetExecutionParam(PLVI::Finish::TIMEOUT_MOVE_MS, 7000);

        // Vision positions for safe Z and home locations
        VMF::VisionPosition safeZPos;
        safeZPos.pos = { 0.0, 0.0, 0.0 };
        safeZPos.locateId = 0;
        safeZPos.visionRequestId = 4;   // safe‑Z position
        params.visionPositions.push_back(safeZPos);

        VMF::VisionPosition homePos;
        homePos.pos = { 0.0, 0.0, 0.0 };
        homePos.locateId = 0;
        homePos.visionRequestId = 5;    // home position
        params.visionPositions.push_back(homePos);

        return params;
    }

    /* Common Vision parameters shared by all Tasks */
    VMF::TaskParams SetPlate1PLVIStrategy::GetVisionParams() const
    {
        VMF::TaskParams params;

        // Common Vision parameters (only keys defined in VisionParamKeys.h)
        params.SetExecutionParam(PLVI::PLVI_POSITION, "0");
        params.SetExecutionParam(PLVI::PKG_NAME, "TEST_PKG");
        params.SetExecutionParam(PLVI::DATA_ID, "1");

        int intX = 8, intY = 4;
        params.SetExecutionParam(PLVI::CTRAY_X, std::to_string(intX));
        params.SetExecutionParam(PLVI::CTRAY_Y, std::to_string(intY));

        // Device info per pocket (default = 99)
        const int totalPockets = intX * intY;
        for (int i = 0; i < totalPockets; ++i)
        {
            std::string key = std::string(PLVI::DEVICE_INFO_PREFIX) + std::to_string(i);
            params.SetExecutionParam(key, "99");
        }

        return params;
    }

}
