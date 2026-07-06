// dllmain.cpp : VMFEquipmentPlugin DLL의 진입점
// Equipment App은 import library(.lib)를 통한 암시적 링크로 이 DLL을 사용합니다.
#include "pch.h"

#include "include/PluginFactory.h"

// !!! 수정 가이드 !!!
// 아래 include를 장비의 실제 Strategy 클래스로 변경하세요.
// 예: #include "Strategies/CLoad1LeftPlateJIGFocusCheckSequenceStrategy.h"
#include "include/SampleSequenceStrategy.h"


// ============================================================================
// Plugin Factory Implementation
// ============================================================================

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


/// <summary>
/// Plugin의 Strategy를 주입한 Orchestrator를 생성합니다.
/// Equipment는 반환된 Orchestrator를 직접 RunSequence / ExecuteDirectVisionCommand 등으로 사용합니다.
/// 
/// 상태머신(Sequence) 모드와 직접(Direct) 모드 모두 사용 가능:
/// - 상태머신 모드: IStrategySetup (IComponentSetup + ISequenceSetup 통합) 주입
/// - 직접 모드: 동일한 인터페이스로 처리되며, Builder/시퀀스는 미사용
/// </summary>
VMFEQUIPMENTPLUGIN_API std::shared_ptr<VMF::Orchestrator> CreateOrchestrator()
{
    auto strategy = std::make_shared<VMF_Sample::SampleSequenceStrategy>();
    // IStrategySetup 전용 생성자 사용 — 동일 객체 1회만 전달
    return std::make_shared<VMF::Orchestrator>(strategy);
}

/// <summary>
/// Orchestrator를 직접 모드(Direct Mode)로 초기화합니다.
/// Plugin 내부의 Strategy를 사용하여 Repository, VisionProcessor, Context를 생성합니다.
/// </summary>
VMFEQUIPMENTPLUGIN_API bool InitializeOrchestratorDirect(std::shared_ptr<VMF::Orchestrator> orch)
{
    if (!orch)
        return false;

    // Plugin의 Strategy를 새로 생성하여 InitializeDirect에 전달
    auto strategy = std::make_shared<VMF_Sample::SampleSequenceStrategy>();
    return orch->InitializeDirect(strategy);
}

VMF::DefaultSetupStrategy* CreateSetupStrategy()
{
	// !!! 수정 필요: 장비의 실제 Strategy 클래스로 변경 !!!
	return new VMF_Sample::SampleSequenceStrategy();
}

void DestroySetupStrategy(VMF::DefaultSetupStrategy* ptr)
{
	if (ptr)
	{
		delete ptr;
		ptr = nullptr;
	}
}





