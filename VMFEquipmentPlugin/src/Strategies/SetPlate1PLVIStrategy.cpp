#include "pch.h"
#include "SetPlate1PLVIStrategy.h"

#include "SqliteDataRepository.h"
#include "..\Protocol\VisionPlviProcessor.h"
#include "..\Strategies\SetPlate1PLVISequenceBuilder.h"
#include "ParamKeys.h"
#include <sstream>
#include <vector>

using namespace VMF;
using namespace VMF_PLUGIN;

/* 1️. Sequence name – identifies the measurement sequence */
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
    // Strategy directly injects all required parameters into the Context.
    // This replaces the former Builder-based approach.
    SetTaskParamsByTask(ctx);
}

VMF::TaskParams SetPlate1PLVIStrategy::GetDefaultParams() const
{
    VMF::TaskParams params;

    // Merge Setup, ExecuteScan, and Finish parameter sets
    for (const auto& pair : GetSetupParams().executionParams)
    {
        params.SetExecutionParam(pair.first, pair.second);
    }

    for (const auto& pair : GetExecuteScanParams().executionParams)
    {
        params.SetExecutionParam(pair.first, pair.second);
    }

    for (const auto& pair : GetFinishParams().executionParams)
    {
        params.SetExecutionParam(pair.first, pair.second);
    }

    // Add common Vision parameters (shared across all Tasks)
    for (const auto& pair : GetVisionParams().executionParams)
    {
        params.SetExecutionParam(pair.first, pair.second);
    }

    return params;
}

/* Setup Task – start position and trigger interval */
VMF::TaskParams SetPlate1PLVIStrategy::GetSetupParams() const
{
    VMF::TaskParams params;

    // Execution parameters
    params.SetExecutionParam(ParamKeys::Setup::TIMEOUT_MOVE_MS, 7000);
    params.SetExecutionParam(ParamKeys::Setup::TRIGGER_INTERVAL_MM, 2.0);

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
    params.SetExecutionParam(ParamKeys::ExecuteScan::TIMEOUT_MOVE_MS, 7000);
    params.SetExecutionParam(ParamKeys::ExecuteScan::TIMEOUT_RESULT_MS, 10000);

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
    params.SetExecutionParam(ParamKeys::Finish::TIMEOUT_MOVE_MS, 7000);

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

    // Common Vision parameters (hand, package, device info, etc.)
    params.SetExecutionParam(ParamKeys::Vision::HAND_ID, "1");
    params.SetExecutionParam(ParamKeys::Vision::PKG_ID, "1");
    params.SetExecutionParam(ParamKeys::Vision::PLVI_POSITION, "0");
    params.SetExecutionParam(ParamKeys::Vision::TIMEOUT_MEASURE_MS, "5000");
    params.SetExecutionParam(ParamKeys::Vision::TIMEOUT_SCAN_MS, "15000");
    params.SetExecutionParam(ParamKeys::Vision::TIMEOUT_RESULT_MS, "10000");
    params.SetExecutionParam(ParamKeys::Vision::TIMEOUT_MOVE_MS, "7000");
    params.SetExecutionParam(ParamKeys::Vision::SCAN_SPEED_MM_S, "100.0");
    params.SetExecutionParam(ParamKeys::Vision::TRIGGER_INTERVAL_MM, "2.0");
    params.SetExecutionParam(ParamKeys::Vision::DATA_ID, "1");
    params.SetExecutionParam(ParamKeys::Vision::PKG_NAME, "TEST_PKG");

    int intX = 8, intY = 4;
    params.SetExecutionParam(ParamKeys::Vision::CTRAY_X, std::to_string(intX));
    params.SetExecutionParam(ParamKeys::Vision::CTRAY_Y, std::to_string(intY));

    // Device info per pocket (default = 99)
    const int totalPockets = intX * intY;
    for (int i = 0; i < totalPockets; ++i)
    {
        std::string key = std::string(ParamKeys::Vision::DEVICE_INFO_PREFIX) + std::to_string(i);
        params.SetExecutionParam(key, "99");
    }

    return params;
}

/* 5️ SetTaskParamsByTask – inject default parameters into Context */
void SetPlate1PLVIStrategy::SetTaskParamsByTask(VMF::Context& ctx) const
{
    // Integrated default parameters (including Vision params)
    VMF::TaskParams defaultParams = GetDefaultParams();
    ctx.SetTaskParams(defaultParams);  // store for later retrieval

    // Task‑specific parameter sets
    SetTaskParamsForTask(ctx, "Task_PLVI_Setup", GetSetupParams());
    SetTaskParamsForTask(ctx, "Task_PLVI_ExecuteScan", GetExecuteScanParams());
    SetTaskParamsForTask(ctx, "Task_PLVI_Finish", GetFinishParams());
}

/* 6️ Helper: set a single Task’s parameters by name */
void SetPlate1PLVIStrategy::SetTaskParamsForTask(VMF::Context& ctx,
    const std::string& taskName,
    const VMF::TaskParams& params) const
{
    ctx.SetTaskParams(taskName, params);
}
