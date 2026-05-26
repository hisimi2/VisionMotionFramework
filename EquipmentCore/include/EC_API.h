#pragma once

#ifdef EQUIPMENTCORE_EXPORTS
#define EQUIPMENT_API __declspec(dllexport)
#else
#define EQUIPMENT_API __declspec(dllimport)
#endif
