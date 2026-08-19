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

std::string SetPlate1PLVIStrategy::GetSequenceName() const
{
    return "SetPlate1PLVI";
}

VMF::DataRepositoryPtr SetPlate1PLVIStrategy::CreateRepository()
{
    auto repo = std::make_shared<VMF::SqliteDataRepository>(
        "Data\\PLVI_DATABASE.db", "Data\\Images");
    repo->Initialize();
    return repo;
}

VMF::VisionProcessorPtr SetPlate1PLVIStrategy::CreateVisionProcessor()
{
    VMF::VisionConnectionConfig config("127.0.0.1", 8000, 3000);
    auto vm = std::make_shared<VisionPlviProcessor>();
    vm->Initialize(config);
    return vm;
}

VMF::SequenceBuilderPtr SetPlate1PLVIStrategy::CreateBuilder()
{
    // Builder는 Task 조립만 담당 (파라미터 설정은 Strategy::ConfigureContext()에서 수행)
    // Builder에 Strategy 참조를 전달할 필요 없음 (m_strategy 멤버 제거됨)
    return std::make_shared<VMF_PLUGIN::SetPlate1PLVISequenceBuilder>();
}

void SetPlate1PLVIStrategy::ConfigureParams(VMF::VisionContextPtr ctx)
{
    // 전략 레벨 파라미터 설정
    // 이 메서드는 Repository에 영구 저장되어야 하는 파라미터를 처리하는 데 사용됩니다.
    // 
    // 실행 시 필요한 런타임 파라미터(TIMEOUT_*, SCAN_END_Y, SCAN_SPEED_MM_S 등)는
    // SetPlate1PLVISequenceBuilder::ConfigureContext()에서 Context에 설정됩니다.
    // 
    // 현재 구현에서는 실행 파라미터를 설정하지 않으며, 인터페이스 준수를 위해 유지됩니다.
    // Repository 저장이 필요한 파라미터(예: Device 정보, PLVI 설정값 등)가 있다면
    // 이 메서드에서 ctx를 통해 Repository에 저장해야 합니다.
    //
    // 예: ctx->GetRepository()->SaveParams(...) 등
    (void)ctx;
}

VMF::StringMap SetPlate1PLVIStrategy::GetVisionParams(const std::string& presetName) const
{
    VMF::StringMap p;
    return VMF::StringMap();
}

// ── PLVI 측정용 기본 Task 파라미터 ──
VMF::TaskParams SetPlate1PLVIStrategy::GetDefaultTaskParams() const
{
    VMF::TaskParams params;
    
    // Task별 파라미터 추가
    VMF::TaskParams setupParams = GetDefaultSetupParams();
    for (const auto& pair : setupParams.executionParams)
    {
        params.SetExecutionParam(pair.first, pair.second);
    }
    params.visionPositions = setupParams.visionPositions;
    
    VMF::TaskParams executeScanParams = GetDefaultExecuteScanParams();
    for (const auto& pair : executeScanParams.executionParams)
    {
        params.SetExecutionParam(pair.first, pair.second);
    }
    
    VMF::TaskParams finishParams = GetDefaultFinishParams();
    for (const auto& pair : finishParams.executionParams)
    {
        params.SetExecutionParam(pair.first, pair.second);
    }
    
    // 공통 Vision 파라미터 추가
    VMF::TaskParams visionParams = GetDefaultVisionParams();
    for (const auto& pair : visionParams.executionParams)
    {
        params.SetExecutionParam(pair.first, pair.second);
    }
    
    return params;
}

// ── Setup Task 전용 기본 파라미터 ──
VMF::TaskParams SetPlate1PLVIStrategy::GetDefaultSetupParams() const
{
    VMF::TaskParams params;
    
    // Setup Task 전용 파라미터
    params.SetExecutionParam(ParamKeys::Setup::TIMEOUT_MOVE_MS, 7000);
    params.SetExecutionParam(ParamKeys::Setup::TRIGGER_INTERVAL_MM, 2.0);
    
    // VisionPositions 설정 (시작 위치)
    VMF::VisionPosition startPos;
    startPos.pos = {0.0, 0.0, 0.0};  // X=0, Y=0, Z=0
    startPos.locateId = 0;
    startPos.visionRequestId = 1;
    params.visionPositions.push_back(startPos);
    
    return params;
}

// ── ExecuteScan Task 전용 기본 파라미터 ──
VMF::TaskParams SetPlate1PLVIStrategy::GetDefaultExecuteScanParams() const
{
    VMF::TaskParams params;
    
    // ExecuteScan Task 전용 파라미터
    params.SetExecutionParam(ParamKeys::ExecuteScan::TIMEOUT_MOVE_MS, 7000);
    params.SetExecutionParam(ParamKeys::ExecuteScan::TIMEOUT_RESULT_MS, 10000);
    params.SetExecutionParam(ParamKeys::ExecuteScan::SCAN_END_Y, 200.0);
    
    return params;
}

// ── Finish Task 전용 기본 파라미터 ──
VMF::TaskParams SetPlate1PLVIStrategy::GetDefaultFinishParams() const
{
    VMF::TaskParams params;
    
    // Finish Task 전용 파라미터
    params.SetExecutionParam(ParamKeys::Finish::TIMEOUT_MOVE_MS, 7000);
    
    return params;
}

// ── 공통 Vision 파라미터 기본값 ──
VMF::TaskParams SetPlate1PLVIStrategy::GetDefaultVisionParams() const
{
    VMF::TaskParams params;
    
    // 공통 Vision 파라미터
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

    int ctrayX = 8, ctrayY = 4;
    params.SetExecutionParam(ParamKeys::Vision::CTRAY_X, std::to_string(ctrayX));
    params.SetExecutionParam(ParamKeys::Vision::CTRAY_Y, std::to_string(ctrayY));

    // Handler 포켓별 Device 정보 (기본값 Device 종류 = 99)
    const int totalPockets = ctrayX * ctrayY;
    for (int i = 0; i < totalPockets; ++i)
    {
        std::string key = std::string(ParamKeys::Vision::DEVICE_INFO_PREFIX) + std::to_string(i);
        params.SetExecutionParam(key, "99");
    }
    
    return params;
}

// ── Task별 파라미터 분리 설정 ──
void SetPlate1PLVIStrategy::SetTaskParamsByTask(VMF::Context& ctx) const
{
    // 기본 파라미터 (공통 Vision 파라미터 포함)
    VMF::TaskParams defaultParams = GetDefaultTaskParams();
    ctx.SetTaskParams(defaultParams);  // 하위 호환성용 기본 파라미터

    // Task별 파라미터 설정 (헬퍼 메서드 사용)
    SetTaskParamsForTask(ctx, "Task_PLVI_Setup", GetDefaultSetupParams());
    SetTaskParamsForTask(ctx, "Task_PLVI_ExecuteScan", GetDefaultExecuteScanParams());
    SetTaskParamsForTask(ctx, "Task_PLVI_Finish", GetDefaultFinishParams());
}

// ── Task별 파라미터 설정 헬퍼 메서드 ──
void SetPlate1PLVIStrategy::SetTaskParamsForTask(VMF::Context& ctx, 
    const std::string& taskName, const VMF::TaskParams& params) const
{
    ctx.SetTaskParams(taskName, params);
}

// ── 4단계 리팩토링: Builder 없이 Context에 직접 파라미터 설정 ──
void SetPlate1PLVIStrategy::ConfigureContext(VMF::Context& ctx)
{
    // Builder 중간 단계 없이 Strategy가 직접 Context에 파라미터를 설정
    // SetTaskParamsByTask()를 호출하여 Task별 파라미터를 Context에 설정
    SetTaskParamsByTask(ctx);
}


