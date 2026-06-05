#pragma once
#include "Actuators\CAxisController.h"
#include "Actuators\CylinderTypes.h"
#include "Actuators\CLamp.h"
#include <vector>

class Load1Parts
{
public:

    Load1Parts();
    ~Load1Parts();

    // 주요 축
    OneAxis                             AxisX;
    OneAxis                             AxisY;
    OneAxis                             AxisZ;
    OneAxis                             AxisTable1;
    OneAxis                             AxisTable2;

    // 실린더
    CForBackCylinder                    CylBuffer;
    CNarrowWideCylinder                 CylYPitch;

    // Setplate / Transfer clamp
    std::vector<CUpDownCylinder>        CylSetplate;
    std::vector<CClampReleaseCylinder>  CylTransfer;
    
    // Pick & Place 확장용 Vacuum/Blow
    std::vector<CVaccumBlowCylinder>    PickVacuum;

    // Lamps
    CLamp                               LampLeft;
    CLamp                               LampRight;
    CLamp                               LampLower;
};

