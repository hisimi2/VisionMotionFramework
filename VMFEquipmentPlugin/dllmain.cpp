// dllmain.cpp : VMFEquipmentPlugin DLL의 진입점
// Equipment App은 import library(.lib)를 통한 암시적 링크로 이 DLL을 사용합니다.
#include "pch.h"

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
