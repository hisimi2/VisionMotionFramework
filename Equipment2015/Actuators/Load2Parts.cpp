#include "stdafx.h"
#include "Load2Parts.h"
#include <sstream>
#include "Utils.h"

static const int SETPLATE_COUNT = 2;
static const int TRANSFERCLAMP_COUNT = 2;

Load2Parts::Load2Parts()
    : AxisX("LOAD2_X")
    , AxisZ("LOAD2_Z")
    , CylXPitch("LOAD2_X_PITCH")
    , CylRail("LoadSideRail")
    , CylPusher("LoadPusher")
    , CylPreciser("LoadPreciser")
{

    // Vacuum 초기화
    PickVacuum.clear();
    for (int i = 0; i < TRANSFERCLAMP_COUNT; ++i)
    {
        PickVacuum.emplace_back(std::string("PickVacuum_") + EC::IntToString(i));
    }
}

Load2Parts::~Load2Parts(){}


