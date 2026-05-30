#pragma once
#include "EC_API.h"
#include "Logger.h"
#include <string>

namespace EC
{
    EC_API std::string IntToString(int v);
    EC_API std::string ToLowerCopy(const std::string& s);

    // 헤더 구현 시 중복 심볼 생성을 막기 위해 static -> inline 변경
    EC_API std::string makeLogPrefix(const std::string& seqName);
    EC_API void LogTask(const std::string& msg);
    EC_API void LogError(const std::string& msg);
}
