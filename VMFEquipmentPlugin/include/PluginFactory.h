#pragma once

#include <memory>
#include "Orchestrator.h"
#include "VMFEquipmentPluginExport.h"

// ============================================================================
// VMFEquipmentPlugin — Orchestrator 조립 팩토리 인터페이스
//
// VMFEquipmentPlugin.dll (암시적 링크) — Equipment App이 import library(.lib)를
// 통해 직접 함수를 호출합니다. LoadLibrary / GetProcAddress 불필요.
//
// 사용 흐름:
//   1. CreateOrchestrator() → Strategy를 생성하고 Orchestrator에 주입하여 반환
//   2. 시퀀스 모드: orch->RunSequence(actuator, config)
//   3. 직접 모드:   InitializeOrchestratorDirect(orch)
//                   orch->ExecuteDirectVisionCommand(VMF::Measure)
//
// 장점: Equipment는 Orchestrator 인터페이스만 알면 되고,
//       구체적인 Strategy/Builder 클래스에 의존하지 않습니다.
//       Plugin DLL 교체 시 Equipment 코드 변경 불필요.
// ============================================================================

/// <summary>
/// Plugin의 Strategy(ComponentSetup + SequenceSetup)를 주입한
/// Orchestrator를 생성하여 반환합니다.
/// </summary>
VMFEQUIPMENTPLUGIN_API std::shared_ptr<VMF::Orchestrator> CreateOrchestrator();

/// <summary>
/// Orchestrator를 직접 모드(Direct Mode)로 초기화합니다.
/// Plugin 내부의 Strategy를 사용하여 Repository, VisionProcessor, Context를 생성합니다.
/// </summary>
VMFEQUIPMENTPLUGIN_API bool InitializeOrchestratorDirect(std::shared_ptr<VMF::Orchestrator> orch);

/// <summary>
/// VMF::DefaultSetupStrategy* 객체를 생성하여 반환합니다.
/// (하위 호환성 유지)
/// </summary>
VMFEQUIPMENTPLUGIN_API VMF::DefaultSetupStrategy* CreateSetupStrategy();

/// <summary>
/// CreateSetupStrategy로 생성된 객체를 소멸합니다.
/// (하위 호환성 유지)
/// </summary>
VMFEQUIPMENTPLUGIN_API void DestroySetupStrategy(VMF::DefaultSetupStrategy* ptr);
