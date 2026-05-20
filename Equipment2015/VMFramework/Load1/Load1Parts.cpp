#include "stdafx.h"
#include "Load1Parts.h"
#include <sstream>

namespace VMF_Load1
{
    // 구성 크기(필요 시 변경)
    static const int LOADPRECISER_SUB_COUNT = 4;
    static const int SETPLATE_COUNT = 2;
    static const int TRANSFERCLAMP_COUNT = 2;

    static std::string IntToString(int v)
    {
        std::ostringstream ss;
        ss << v;
        return ss.str();
    }

    Load1Parts::Load1Parts()
        : LOAD1_X("LOAD1_X"), LOAD1_Y("LOAD1_Y"), LOAD1_Z("LOAD1_Z")
        , LOAD_TABLE1("LOAD_TABLE1"), LOAD_TABLE2("LOAD_TABLE2"), LoadHandYPitch("LOAD_Y_PITCH")
        , LoadBuffer("LoadBuffer"), LoadRail("LoadRail"), LoadPusher("LoadPusher")
        , LoadPreciser("LoadPreciser")
        , VisionLED_Left("VisionLED_Left")
        , VisionLED_Right("VisionLED_Right")
        , VisionLED_Lower("VisionLED_Lower")
    {
        // LoadPreciserSub 초기화
        LoadPreciserSub.clear();
        for (int i = 0; i < LOADPRECISER_SUB_COUNT; ++i)
        {
            LoadPreciserSub.emplace_back(std::string("LoadPreciserSub_") + IntToString(i));
        }

        // Setplate 초기화
        Setplate.clear();
        for (int i = 0; i < SETPLATE_COUNT; ++i)
        {
            Setplate.emplace_back(std::string("Setplate_") + IntToString(i));
        }

        // TransferClamp 초기화
        TransferClamp.clear();
        for (int i = 0; i < TRANSFERCLAMP_COUNT; ++i)
        {
            TransferClamp.emplace_back(std::string("TransferClamp_") + IntToString(i));
        }
    }

    Load1Parts::~Load1Parts(){}

} // namespace Actuators
