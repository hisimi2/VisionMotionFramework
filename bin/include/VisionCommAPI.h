#pragma once

#if defined(VC_STATIC_LIB)
    #define VC_API
#elif defined(VISION_COMM_EXPORTS)
    #define VC_API __declspec(dllexport)
#else
    #define VC_API __declspec(dllimport)
#endif

