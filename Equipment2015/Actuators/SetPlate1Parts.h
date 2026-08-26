#pragma once

#include "CAxisController.h"
#include "CylinderTypes.h"
#include "CLamp.h"
#include <vector>

class SetPlate1Parts
{
public:
	SetPlate1Parts()
        : AxisY("AXIS_Y")
        , AxisZ("AXIS_Z")
        , CylSetplateClamp()
        , CylSetplateUpDown()
        , LampPLVI("LAMP_MAIN")
        , LampSide("LAMP_SIDE")
	{};

	~SetPlate1Parts() {};

	OneAxis             AxisY;          
	OneAxis             AxisZ;          

	std::vector<CClampReleaseCylinder>  CylSetplateClamp;
	std::vector<CUpDownCylinder>        CylSetplateUpDown;

	CLamp               LampPLVI;
	CLamp               LampSide;

};
