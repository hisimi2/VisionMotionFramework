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
    auto builder = std::make_shared<VMF_PLUGIN::SetPlate1PLVISequenceBuilder>();
    
    // Strategy가 기본 파라미터를 Builder에 설정
    // 이를 통해 Builder는 Strategy의 기본 파라미터를 Context에 적용할 수 있습니다.
    builder->SetTaskParams(GetDefaultTaskParams());
    
    return builder;
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

    // ── Setup Task 전용 파라미터 ──
    params.SetExecutionParam(ParamKeys::Setup::TIMEOUT_MOVE_MS, 7000);
    params.SetExecutionParam(ParamKeys::Setup::TRIGGER_INTERVAL_MM, 2.0);

    // ── ExecuteScan Task 전용 파라미터 ──
    params.SetExecutionParam(ParamKeys::ExecuteScan::TIMEOUT_MOVE_MS, 7000);
    params.SetExecutionParam(ParamKeys::ExecuteScan::TIMEOUT_RESULT_MS, 10000);
    params.SetExecutionParam(ParamKeys::ExecuteScan::SCAN_END_Y, 200.0);

    // ── Finish Task 전용 파라미터 ──
    params.SetExecutionParam(ParamKeys::Finish::TIMEOUT_MOVE_MS, 7000);

    // ── 공통 Vision 파라미터 ──
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

    // ── VisionPositions 설정 (시작 위치) ──
    VMF::VisionPosition startPos;
    startPos.pos = {0.0, 0.0, 0.0};  // X=0, Y=0, Z=0
    startPos.locateId = 0;
    startPos.visionRequestId = 1;
    params.visionPositions.push_back(startPos);

    return params;
}


