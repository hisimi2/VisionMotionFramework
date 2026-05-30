#pragma once

#ifdef EQUIPMENTCORE_EXPORTS
#define EC_API __declspec(dllexport)
#else
#define EC_API __declspec(dllimport)
#endif
