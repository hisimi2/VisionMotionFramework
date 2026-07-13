#pragma once

#include "CAxisController.h"
#include "CylinderTypes.h"
#include "CLamp.h"
#include <vector>

// PLVI 동작 구성:

class SetPlate1Parts
{
public:
	SetPlate1Parts() : AxisY("AXIS_Y"), AxisZ("AXIS_Z"),
		CylSetplateClamp(), CylSetplateUpDown(),
		LampPLVI("LAMP_MAIN"), LampSide("LAMP_SIDE")
	{};
	~SetPlate1Parts() {};

	OneAxis             AxisY;          // PLVI 스캔 방향 축 
	OneAxis             AxisZ;          // Z축 

	std::vector<CClampReleaseCylinder>  CylSetplateClamp;
	std::vector<CUpDownCylinder>        CylSetplateUpDown;

	CLamp               LampPLVI;
	CLamp               LampSide;

};
