#pragma once

#include <memory>
#include "ComponentSetupBase.h"

// ============================================================================
// [Sample] DLL Plugin Factory Interface
//
// 메인 APP에서 LoadLibrary로 DLL을 로드한 후,
// GetProcAddress로 CreateSetupStrategy / DestroySetupStrategy를 호출하여
// VMF::ComponentSetupBase 객체를 생성/소멸합니다.
//
// !!! 수정 가이드 !!!
// 1. CreateSetupStrategy 내부에서 반환하는 Strategy 클래스를
//    장비의 실제 Strategy 클래스로 변경 (예: VMF_Sample::SampleSequenceStrategy)
// 2. 필요하다면 CreateSetupStrategy에 파라미터를 추가하여
//    장비 타입을 전달받아 분기 처리 가능
// 3. DLL 외부로 노출되므로 __declspec(dllexport) 유지
// ============================================================================

extern "C"
{
	/// <summary>
	/// VMF::ComponentSetupBase* 객체를 생성하여 반환합니다.
	/// 메인 APP은 이 포인터로 시퀀스를 초기화/실행합니다.
	/// </summary>
	__declspec(dllexport) VMF::ComponentSetupBase* CreateSetupStrategy();

	/// <summary>
	/// CreateSetupStrategy로 생성된 객체를 소멸합니다.
	/// </summary>
	__declspec(dllexport) void DestroySetupStrategy(VMF::ComponentSetupBase* ptr);
}
