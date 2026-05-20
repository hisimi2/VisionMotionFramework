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

namespace VMF_Load1
{
    class Load1Parts
    {
    public:
        Load1Parts();
        ~Load1Parts();

        // 주요 축
        OneAxis                             LOAD1_X;
        OneAxis                             LOAD1_Y;
        OneAxis                             LOAD1_Z;
        OneAxis                             LOAD_TABLE1;
        OneAxis                             LOAD_TABLE2;

        // Lamps / LEDs
        CLamp                               VisionLED_Left;
        CLamp                               VisionLED_Right;
        CLamp                               VisionLED_Lower;

        // 실린더
        CForBackCylinder                    LoadBuffer;
        COpenCloseCylinder                  LoadRail;
        CForBackCylinder                    LoadPusher;
        CUpDownCylinder                     LoadPreciser;
        CNarrowWideCylinder                 LoadHandYPitch;

        // Setplate / Transfer clamp
        std::vector<CUpDownCylinder>        Setplate;
        std::vector<CClampReleaseCylinder>  TransferClamp;
        std::vector<CUpDownCylinder>        LoadPreciserSub;
    };
}
