#include "stdafx.h"
#include "Utils.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace EC
{
    std::string IntToString(int v)
    {
        std::ostringstream ss;
        ss << v;
        return ss.str();
    }

    std::string ToLowerCopy(const std::string& s)
    {
        std::string out = s;
        std::transform(out.begin(), out.end(), out.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return out;
    }
}
