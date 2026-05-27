#include "stdafx.h"
#include "Load1Parts.h"

#include "Utils.h"
#include <sstream>

static const int SETPLATE_COUNT = 2;
static const int TRANSFER_COUNT = 2;
static const int VACCUM_COUNT   = 24;

Load1Parts::Load1Parts()
    : AxisX("LOAD1_X")
    , AxisY("LOAD1_Y")
    , AxisZ("LOAD1_Z")
    , AxisTable1("LOAD_TABLE1")
    , AxisTable2("LOAD_TABLE2")
    , CylBuffer("LoadBuffer")
    , CylYPitch("LOAD_Y_PITCH")
    , LampLeft("VisionLED_Left")
    , LampRight("VisionLED_Right")
    , LampLower("VisionLED_Lower")
{
    // Setplate 초기화
    CylSetplate.clear();
    for (int i = 0; i < SETPLATE_COUNT; ++i)
    {
        CylSetplate.emplace_back(std::string("Setplate_") + EC::IntToString(i));
    }

    // TransferClamp 초기화
    CylTransfer.clear();
    for (int i = 0; i < TRANSFER_COUNT; ++i)
    {
        CylTransfer.emplace_back(std::string("TransferClamp_") + EC::IntToString(i));
    }

    // Vacuum 초기화
    PickVacuum.clear();
    for (int i = 0; i < VACCUM_COUNT; ++i)
    {
        PickVacuum.emplace_back(std::string("PickVacuum_") + EC::IntToString(i));
    }
}

Load1Parts::~Load1Parts(){}


