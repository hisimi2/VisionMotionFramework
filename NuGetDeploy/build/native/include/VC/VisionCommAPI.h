#pragma once

// VisionComm.lib는 정적 라이브러리로 배포됩니다.
// NuGet 패키지의 .targets 파일이 VC_STATIC_LIB를 자동 정의합니다.
//
// 사용 시나리오:
//   1) VC_STATIC_LIB가 정의됨 -> VISION_COMM_API는 빈 매크로 (static lib consumer)
//   2) VISION_COMM_EXPORTS가 정의됨 -> VISION_COMM_API __declspec(dllexport) (라이브러리 빌드 시)
//   3) 둘 다 없음 -> VISION_COMM_API __declspec(dllimport) (fallback, 사용 안 함)

#if defined(VC_STATIC_LIB)
    #define VISION_COMM_API
#elif defined(VISION_COMM_EXPORTS)
    #define VISION_COMM_API __declspec(dllexport)
#else
    #define VISION_COMM_API __declspec(dllimport)
#endif

