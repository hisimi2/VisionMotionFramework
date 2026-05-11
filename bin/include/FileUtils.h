#pragma once

#include <string>
#include <ctime>
#include <sstream>
#include <vector>

namespace VMF
{
    class FileUtils
    {
    public:
        static std::string MakeParamPath(const std::string& basePath, const std::string& recipe) {
            return JoinPath(basePath, recipe + ".params");
        }

        static std::string MakeImagePath(const std::string& basePath, const std::string& contextTag) {
            std::stringstream ss;
            ss << contextTag << "_" << MakeTimeTag() << ".raw";
            return JoinPath(basePath, ss.str());
        }

        static std::string MakeTimeTag()
        {
            // C++11: NULL -> nullptr
            std::time_t t = std::time(nullptr);
            struct tm tmBuf;

            localtime_s(&tmBuf, &t);
            char buf[64] = {0};
            if (std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tmBuf) == 0) {
                return "unknown_time";
            }
            return std::string(buf);
        }

        // 경로를 OS에 맞게 합쳐줌 (예: "base", "file.bin" -> "base/file.bin")
        static std::string JoinPath(const std::string& base, const std::string& name)
        {
            const char sep = '\\';
            if (base.empty()) return name;
            if (name.empty()) return base;
        
            // C++11부터 std::string::back() 지원됨 (C++14 컴파일 가능하므로 안전하게 사용)
            char lastChar = base.back();
            if (lastChar == sep || lastChar == '/')
            {
                 return base + name;
            }
        
            return base + sep + name;
        }
    };

} // namespace VMF
