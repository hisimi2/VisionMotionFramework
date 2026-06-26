#pragma once

#ifdef VISION_COMM_EXPORTS
#define VISION_COMM_API __declspec(dllexport)
#else
#define VISION_COMM_API __declspec(dllimport)
#endif

