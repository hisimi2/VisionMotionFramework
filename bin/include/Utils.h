#pragma once
#include "EC_API.h"

#include <string>

namespace EC
{
    EC_API std::string IntToString(int v);
    EC_API std::string ToLowerCopy(const std::string& s);

    EC_API std::string makeLogPrefix(const std::string& seqName);
    EC_API void LogTask(const std::string& msg);
    EC_API void LogError(const std::string& msg);
}
