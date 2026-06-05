#pragma once
#include "Actuators\CAxisController.h"
#include "Actuators\CylinderTypes.h"
#include "Actuators\CLamp.h"
#include <vector>

class Load2Parts
{
public:
    Load2Parts();
    ~Load2Parts();

    // 주요 축
    OneAxis                             AxisX;
    OneAxis                             AxisZ;

    // 실린더
    CNarrowWideCylinder                 CylXPitch;
    COpenCloseCylinder                  CylRail;
    CForBackCylinder                    CylPusher;
    CUpDownCylinder                     CylPreciser;
  
    // Vacuum/Blow
    std::vector<CVaccumBlowCylinder>    PickVacuum;
};

