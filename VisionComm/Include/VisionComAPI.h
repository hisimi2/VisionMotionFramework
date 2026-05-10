#pragma once

#ifdef VISION_COM_EXPORTS
#define VISION_COM_API __declspec(dllexport)
#else
#define VISION_COM_API __declspec(dllimport)
#endif\n// Backwards-compatibility: alias old namespace name to new one\nnamespace VisionComm {}\nnamespace VisionCom = VisionComm;\n
