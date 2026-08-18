#pragma once

// VMF.Core는 정적 라이브러리(.lib)로 배포됩니다.
// NuGet 패키지의 .targets 파일이 VMF_STATIC_LIB를 자동 정의합니다.
//
// 사용 시나리오:
//   1) VMF_STATIC_LIB가 정의됨 -> VMF_API는 빈 매크로 (static lib consumer)
//   2) VMF_EXPORTS가 정의됨   -> VMF_API __declspec(dllexport)  (라이브러리 빌드 시)
//   3) 둘 다 없음             -> VMF_API __declspec(dllimport)  (fallback, 사용 안 함)

#if defined(VMF_STATIC_LIB)
    #define VMF_API
#elif defined(VMF_EXPORTS)
    #define VMF_API __declspec(dllexport)
#else
    #define VMF_API __declspec(dllimport)
#endif
