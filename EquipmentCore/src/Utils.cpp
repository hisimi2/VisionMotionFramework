#include "stdafx.h"
#include "Utils.h"
#include <sstream>

namespace EC
{
    std::string IntToString(int v)
    {
        std::ostringstream ss;
        ss << v;
        return ss.str();
    }
}
