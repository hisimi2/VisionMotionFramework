#pragma once
#include "Actuators\CAxisController.h"
#include "Actuators\CUpDownCylinder.h"
#include "Actuators\CForBackCylinder.h"
#include "Actuators\COpenCloseCylinder.h"
#include "Actuators\CClampReleaseCylinder.h"
#include "Actuators\CVaccumBlowCylinder.h"
#include "Actuators\CNarrowWideCylinder.h"
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

