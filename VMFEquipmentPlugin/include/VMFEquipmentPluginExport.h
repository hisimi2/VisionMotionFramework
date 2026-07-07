#pragma once

// ============================================================================
// VMFEquipmentPlugin DLL Export/Import Macro
//
// VMFEQUIPMENTPLUGIN_EXPORTS는 프로젝트 설정에 정의되어 있으며,
// VMFEquipmentPlugin.dll 빌드 시에만 __declspec(dllexport)로 정의됩니다.
// 외부 소비 프로젝트(Equipment App)에서는 __declspec(dllimport)로 자동 전환됩니다.
// ============================================================================

#ifdef VMFEQUIPMENTPLUGIN_EXPORTS
    #define VMF_PLUGIN_API __declspec(dllexport)
#else
    #define VMF_PLUGIN_API __declspec(dllimport)
#endif
