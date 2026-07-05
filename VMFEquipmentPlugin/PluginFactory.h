#pragma once

#include <memory>
#include "DefaultSetupStrategy.h"
#include "VMFEquipmentPluginExport.h"

// ============================================================================
// [Sample] SetupStrategy Factory Interface
//
// VMFEquipmentPlugin.dll (암시적 링크) — Equipment App이 import library(.lib)를
// 통해 직접 함수 및 클래스를 호출합니다.
// LoadLibrary / GetProcAddress 불필요.
//
// CreateSetupStrategy() / DestroySetupStrategy() — DLL에 export된 팩토리 함수
//
// !!! 수정 가이드 !!!
// 1. CreateSetupStrategy 내부에서 반환하는 Strategy 클래스를
//    장비의 실제 Strategy 클래스로 변경 (예: VMF_Sample::SampleSequenceStrategy)
// 2. 필요하다면 CreateSetupStrategy에 파라미터를 추가하여
//    장비 타입을 전달받아 분기 처리 가능
// 3. 새 Strategy 클래스를 추가할 때는 클래스 선언에 VMFEQUIPMENTPLUGIN_API를
//    추가하면 export 함수 없이도 Equipment App에서 직접 사용 가능
// ============================================================================

/// <summary>
/// VMF::DefaultSetupStrategy* 객체를 생성하여 반환합니다.
/// 메인 APP은 이 포인터로 시퀀스를 초기화/실행합니다.
/// DefaultSetupStrategy는 IComponentSetup + ISequenceSetup을 통합합니다.
/// </summary>
VMFEQUIPMENTPLUGIN_API VMF::DefaultSetupStrategy* CreateSetupStrategy();

/// <summary>
/// CreateSetupStrategy로 생성된 객체를 소멸합니다.
/// </summary>
VMFEQUIPMENTPLUGIN_API void DestroySetupStrategy(VMF::DefaultSetupStrategy* ptr);
