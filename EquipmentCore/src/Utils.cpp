#include "stdafx.h"
#include "Utils.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include "Logger.h"

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

    std::string makeLogPrefix(const std::string& seqName)
    {
        std::ostringstream oss;
        oss << "[Activity:" << seqName << "] ";
        return oss.str();
    }

    // 공정 로그를 위한 단순 출력 헬퍼 -> Logger 연결
    void LogTask(const std::string& msg)
    {
        // Logger를 통해 파일/콘솔 동시 출력
        Logger::Instance().Log(LOG_INFO, msg);
    }

    // (옵션) 에러 로그 헬퍼 추가
    void LogError(const std::string& msg)
    {
        Logger::Instance().Log(LOG_ERROR, msg);
    }
}
