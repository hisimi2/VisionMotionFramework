// dllmain.cpp : VMFEquipmentPlugin DLL의 진입점
// Equipment App은 import library(.lib)를 통한 암시적 링크로 이 DLL을 사용합니다.
#include "pch.h"

#include "PluginFactory.h"

// !!! 수정 가이드 !!!
// 아래 include를 장비의 실제 Strategy 클래스로 변경하세요.
// 예: #include "Strategies/CLoad1LeftPlateJIGFocusCheckSequenceStrategy.h"
#include "Strategies/SampleSequenceStrategy.h"

// ============================================================================
// Plugin Factory Implementation
//
// !!! 수정 가이드 !!!
// VMF_Sample::SampleSequenceStrategy 부분을 장비의 실제 Strategy 클래스로 변경하세요.
//
// 예시 1 - Load1:
//   #include "VMFComposition/Load1/Strategies/CLoad1LeftPlateJIGFocusCheckSequenceStrategy.h"
//   return new VMF_Load1::CLoad1LeftPlateJIGFocusCheckSequenceStrategy();
//
// 예시 2 - Load2:
//   #include "VMFComposition/Load2/Strategies/CLoad2RightPlateFocusCheckSequenceStrategy.h"
//   return new VMF_Load2::CLoad2RightPlateFocusCheckSequenceStrategy();
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

VMF::ComponentSetupBase* CreateSetupStrategy()
{
	// !!! 수정 필요: 장비의 실제 Strategy 클래스로 변경 !!!
	return new VMF_Sample::SampleSequenceStrategy();
}

void DestroySetupStrategy(VMF::ComponentSetupBase* ptr)
{
	if (ptr)
	{
		delete ptr;
		ptr = nullptr;
	}
}


