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

// ═══════════════════════════════════════════════════════════════════
// 시퀀스 식별
// ═══════════════════════════════════════════════════════════════════
std::string SetPlate1PLVIStrategy::GetSequenceName() const
{
    return "SetPlate1PLVI";
}

// ═══════════════════════════════════════════════════════════════════
// 저장소 생성 (SQLite 데이터베이스)
// ═══════════════════════════════════════════════════════════════════
VMF::DataRepositoryPtr SetPlate1PLVIStrategy::CreateRepository()
{
    auto repo = std::make_shared<VMF::SqliteDataRepository>(
        "Data\\PLVI_DATABASE.db", "Data\\Images");
    repo->Initialize();
    return repo;
}

// ═══════════════════════════════════════════════════════════════════
// 비전 프로세서 생성 (VisionPlviProcessor)
// ═══════════════════════════════════════════════════════════════════
VMF::VisionProcessorPtr SetPlate1PLVIStrategy::CreateVisionProcessor()
{
    VMF::VisionConnectionConfig config("127.0.0.1", 8000, 3000);
    auto vm = std::make_shared<VisionPlviProcessor>();
    vm->Initialize(config);
    return vm;
}

// ═══════════════════════════════════════════════════════════════════
// 시퀀스 빌더 생성 (Task 조립만 담당)
// ═══════════════════════════════════════════════════════════════════
VMF::SequenceBuilderPtr SetPlate1PLVIStrategy::CreateBuilder()
{
    // Builder는 Task 조립만 담당 (파라미터 설정은 Strategy::ConfigureContext()에서 수행)
    return std::make_shared<VMF_PLUGIN::SetPlate1PLVISequenceBuilder>();
}

// ═══════════════════════════════════════════════════════════════════
// 전략 레벨 파라미터 설정 (Repository 영구 저장용)
// ═══════════════════════════════════════════════════════════════════
void SetPlate1PLVIStrategy::ConfigureParams(VMF::VisionContextPtr ctx)
{
    // 전략 레벨 파라미터 설정
    // 이 메서드는 Repository에 영구 저장되어야 하는 파라미터를 처리하는 데 사용됩니다.
    // 
    // 실행 시 필요한 런타임 파라미터(TIMEOUT_*, SCAN_END_Y, SCAN_SPEED_MM_S 등)는
    // ConfigureContext()에서 Context에 설정됩니다.
    // 
    // 현재 구현에서는 실행 파라미터를 설정하지 않으며, 인터페이스 준수를 위해 유지됩니다.
    // Repository 저장이 필요한 파라미터(예: Device 정보, PLVI 설정값 등)가 있다면
    // 이 메서드에서 ctx를 통해 Repository에 저장해야 합니다.
    //
    // 예: ctx->GetRepository()->SaveParams(...) 등
    (void)ctx;
}

// ═══════════════════════════════════════════════════════════════════
// 기본 Task 파라미터 정의 (하위 호환용 통합 파라미터)
// ═══════════════════════════════════════════════════════════════════
// 
// 파라미터 설정 흐름:
// ┌─────────────────────────────────────────────────────────────────┐
// │ 1. Strategy가 기본 파라미터 정의 (GetDefault*Params)            │
// │ 2. ConfigureContext() 호출 시 Context에 파라미터 설정           │
// │ 3. 각 Task가 OnInitialize()에서 GetTaskParams(GetName())로 조회 │
// └─────────────────────────────────────────────────────────────────┘
//
// 위치 파라미터 (visionPositions):
// - Setup: visionPositions[0] = 시작 위치 (visionRequestId=1)
// - ExecuteScan: visionPositions[0]=측정 시작, [1]=측정 종료 (visionRequestId=2,3)
// - Finish: visionPositions[0]=안전 Z, [1]=홈 위치 (visionRequestId=4,5)
//
VMF::TaskParams SetPlate1PLVIStrategy::GetDefaultParams() const
{
    VMF::TaskParams params;

    // Task별 파라미터 추가 (실행 파라미터만 통합)
    VMF::TaskParams setupParams = GetSetupParams();
    for (const auto& pair : setupParams.executionParams)
    {
        params.SetExecutionParam(pair.first, pair.second);
    }

    VMF::TaskParams executeScanParams = GetExecuteScanParams();
    for (const auto& pair : executeScanParams.executionParams)
    {
        params.SetExecutionParam(pair.first, pair.second);
    }

    VMF::TaskParams finishParams = GetFinishParams();
    for (const auto& pair : finishParams.executionParams)
    {
        params.SetExecutionParam(pair.first, pair.second);
    }

    // 공통 Vision 파라미터 추가
    VMF::TaskParams visionParams = GetVisionParams();
    for (const auto& pair : visionParams.executionParams)
    {
        params.SetExecutionParam(pair.first, pair.second);
    }

    // VisionPositions는 각 Task별로 분리 관리되므로 GetDefaultParams()에는 포함하지 않음
    // 각 Task는 SetTaskParamsByTask()를 통해 자신의 visionPositions를 Context에서 조회

    return params;
}

// ═══════════════════════════════════════════════════════════════════
// Setup Task 전용 기본 파라미터
// ═══════════════════════════════════════════════════════════════════
//
// Setup Task 역할:
// - Z축 안전 위치로 이동 (MoveSafeZ / WaitSafeZ)
// - 수평 시작 위치로 이동 (MoveHorizontalStart / WaitHorizontalStart)
// - 트리거 및 레이저 설정 (SetupTrigger)
//
// 사용 파라미터:
// - TIMEOUT_MOVE_MS (int): 이동 타임아웃 [ms], 기본값 7000
// - TRIGGER_INTERVAL_MM (double): 트리거 간격 [mm], 기본값 2.0
// - visionPositions[0]: 시작 위치 (VisionPosition, visionRequestId=1)
//
VMF::TaskParams SetPlate1PLVIStrategy::GetSetupParams() const
{
    VMF::TaskParams params;

    // Setup Task 전용 파라미터
    params.SetExecutionParam(ParamKeys::Setup::TIMEOUT_MOVE_MS, 7000);
    params.SetExecutionParam(ParamKeys::Setup::TRIGGER_INTERVAL_MM, 2.0);

    // VisionPositions 설정 (시작 위치)
    VMF::VisionPosition startPos;
    startPos.pos = { 0.0, 0.0, 0.0 };  // X=0, Y=0, Z=0
    startPos.locateId = 0;
    startPos.visionRequestId = 1;
    params.visionPositions.push_back(startPos);

    return params;
}

// ═══════════════════════════════════════════════════════════════════
// ExecuteScan Task 전용 기본 파라미터
// ═══════════════════════════════════════════════════════════════════
//
// ExecuteScan Task 역할:
// - 측정 영역으로 이동 (MoveMeasurementArea / WaitMeasurementArea)
// - 측정 결과 요청 (RequestResult)
// - 측정 결과 수신 대기 (WaitResult)
//
// 사용 파라미터:
// - TIMEOUT_MOVE_MS (int): 이동 타임아웃 [ms], 기본값 7000
// - TIMEOUT_RESULT_MS (int): 결과 수신 타임아웃 [ms], 기본값 10000
// - visionPositions[0]: 측정 시작 위치 (VisionPosition, visionRequestId=2)
// - visionPositions[1]: 측정 종료 위치 (VisionPosition, visionRequestId=3)
//
VMF::TaskParams SetPlate1PLVIStrategy::GetExecuteScanParams() const
{
    VMF::TaskParams params;

    // ExecuteScan Task 전용 파라미터
    params.SetExecutionParam(ParamKeys::ExecuteScan::TIMEOUT_MOVE_MS, 7000);
    params.SetExecutionParam(ParamKeys::ExecuteScan::TIMEOUT_RESULT_MS, 10000);

    // VisionPositions 설정 (측정 시작/종료 위치)
    // visionRequestId: 2=측정 시작 위치, 3=측정 종료 위치
    VMF::VisionPosition scanStartPos;
    scanStartPos.pos = { 0.0, 0.0, 0.0 };
    scanStartPos.locateId = 0;
    scanStartPos.visionRequestId = 2;
    params.visionPositions.push_back(scanStartPos);

    VMF::VisionPosition scanEndPos;
    scanEndPos.pos = { 0.0, 200.0, 0.0 };
    scanEndPos.locateId = 0;
    scanEndPos.visionRequestId = 3;
    params.visionPositions.push_back(scanEndPos);

    return params;
}

// ═══════════════════════════════════════════════════════════════════
// Finish Task 전용 기본 파라미터
// ═══════════════════════════════════════════════════════════════════
//
// Finish Task 역할:
// - Z축 안전 위치로 이동 (MoveSafeZ / WaitSafeZ)
// - 대기(홈) 위치로 이동 (MoveHome / WaitHome)
//
// 사용 파라미터:
// - TIMEOUT_MOVE_MS (int): 이동 타임아웃 [ms], 기본값 7000
// - visionPositions[0]: 안전 Z 위치 (VisionPosition, visionRequestId=4)
// - visionPositions[1]: 홈 위치 (VisionPosition, visionRequestId=5)
//
VMF::TaskParams SetPlate1PLVIStrategy::GetFinishParams() const
{
    VMF::TaskParams params;

    // Finish Task 전용 파라미터
    params.SetExecutionParam(ParamKeys::Finish::TIMEOUT_MOVE_MS, 7000);

    // VisionPositions 설정 (안전 Z/홈 위치)
    // visionRequestId: 4=안전 Z 위치, 5=홈 위치
    VMF::VisionPosition safeZPos;
    safeZPos.pos = { 0.0, 0.0, 0.0 };
    safeZPos.locateId = 0;
    safeZPos.visionRequestId = 4;
    params.visionPositions.push_back(safeZPos);

    VMF::VisionPosition homePos;
    homePos.pos = { 0.0, 0.0, 0.0 };
    homePos.locateId = 0;
    homePos.visionRequestId = 5;
    params.visionPositions.push_back(homePos);

    return params;
}

// ═══════════════════════════════════════════════════════════════════
// 공통 Vision 파라미터 기본값
// ═══════════════════════════════════════════════════════════════════
//
// 모든 Task에서 공통으로 사용하는 Vision 파라미터:
// - HAND_ID, PKG_ID, PLVI_POSITION, DATA_ID, PKG_NAME
// - TIMEOUT_MEASURE_MS, TIMEOUT_SCAN_MS, TIMEOUT_RESULT_MS, TIMEOUT_MOVE_MS
// - SCAN_SPEED_MM_S, TRIGGER_INTERVAL_MM
// - CTRAY_X, CTRAY_Y (Handler 트레이 크기)
// - DEVICE_INFO_0 ~ DEVICE_INFO_31 (Handler 포켓별 Device 정보)
//
VMF::TaskParams SetPlate1PLVIStrategy::GetVisionParams() const
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

// ═══════════════════════════════════════════════════════════════════
// Task별 파라미터 분리 설정
// ═══════════════════════════════════════════════════════════════════
//
// 파라미터 설정 흐름:
// 1. GetDefaultParams()로 하위 호환용 기본 파라미터 설정
// 2. SetTaskParamsForTask()로 Task별 파라미터 설정
//    - "Task_PLVI_Setup": Setup Task 파라미터 (visionPositions[0] 포함)
//    - "Task_PLVI_ExecuteScan": ExecuteScan Task 파라미터 (visionPositions[0,1] 포함)
//    - "Task_PLVI_Finish": Finish Task 파라미터 (visionPositions[0,1] 포함)
// 
// 각 Task는 OnInitialize()에서 다음과 같이 자신의 파라미터를 조회합니다:
//   const auto& taskParams = provider.GetTaskParams(GetName());
//   const auto& positions = provider.GetVisionPositions();
//
// Task별 파라미터 격리 효과:
// - Task 간 파라미터 키 충돌 방지
// - 각 Task가 자신의 파라미터만 조회 가능
// - 위치 파라미터도 Task별로 분리 관리 (visionRequestId로 구분)
//
void SetPlate1PLVIStrategy::SetTaskParamsByTask(VMF::Context& ctx) const
{
    // 기본 파라미터 (공통 Vision 파라미터 포함)
    VMF::TaskParams defaultParams = GetDefaultParams();
    ctx.SetTaskParams(defaultParams);  // 하위 호환성용 기본 파라미터

    // Task별 파라미터 설정 (헬퍼 메서드 사용)
    SetTaskParamsForTask(ctx, "Task_PLVI_Setup", GetSetupParams());
    SetTaskParamsForTask(ctx, "Task_PLVI_ExecuteScan", GetExecuteScanParams());
    SetTaskParamsForTask(ctx, "Task_PLVI_Finish", GetFinishParams());
}

// ═══════════════════════════════════════════════════════════════════
// Task별 파라미터 설정 헬퍼 메서드
// ═══════════════════════════════════════════════════════════════════
//
// 지정된 Task 이름과 파라미터로 Context에 Task별 파라미터를 설정합니다.
// SetTaskParamsByTask()에서 반복 패턴을 추상화하기 위해 사용됩니다.
//
void SetPlate1PLVIStrategy::SetTaskParamsForTask(VMF::Context& ctx,
    const std::string& taskName, const VMF::TaskParams& params) const
{
    ctx.SetTaskParams(taskName, params);
}

// ═══════════════════════════════════════════════════════════════════
// Builder 없이 Context에 직접 파라미터 설정 (4단계 리팩토링)
// ═══════════════════════════════════════════════════════════════════
//
// 사용 방법:
//   auto strategy = SetPlate1PLVIStrategy::Create();
//   strategy->ConfigureContext(ctx);  // Task별 파라미터 Context에 설정
//   
//   // Task 직접 조립
//   auto sequence = std::make_shared<VMF::Sequence>("SetPlate1PLVI");
//   sequence->AddTask(std::make_shared<SetPlate1PLVISetup>());
//   sequence->AddTask(std::make_shared<SetPlate1PLVIExecuteScan>());
//   sequence->AddTask(std::make_shared<SetPlate1PLVIFinish>());
//   
//   // 시퀀스 실행
//   sequence->Execute(ctx, actuator);

// Builder 사용 시 (하위 호환성):
//   auto builder = strategy->CreateBuilder();
//   builder->ConfigureContext(ctx);  // 내부적으로 ConfigureContext() 호출
//   auto sequence = builder->BuildSequence("SetPlate1PLVI");

// 차이점:
// - ConfigureContext(): Strategy가 직접 Context에 파라미터 설정 (권장)
// - CreateBuilder() → ConfigureContext(): Builder를 통한 간접 설정 (하위 호환성)

// 책임 분리:
// - Strategy: 파라미터 정의 + Context에 설정 (ConfigureContext())
// - Builder: Task 조립만 담당 (BuildSequence())
// - Task: Context에서 파라미터 조회 및 사용 (OnInitialize())
//
void SetPlate1PLVIStrategy::ConfigureContext(VMF::Context& ctx)
{
    // Builder 중간 단계 없이 Strategy가 직접 Context에 파라미터를 설정
    // SetTaskParamsByTask()를 호출하여 Task별 파라미터를 Context에 설정
    SetTaskParamsByTask(ctx);
}
