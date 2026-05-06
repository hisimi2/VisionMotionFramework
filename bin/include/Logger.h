#pragma once

#include <string>
#include <fstream>
#include <cstdarg>

// VS2010 호환: <mutex> 없음 -> Win32 CriticalSection 사용
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace DVH_VAT
{
    // VS2010 호환: enum class 미지원, 매크로 충돌 방지를 위한 접두어 사용
    enum LogLevel
    {
        LOG_DEBUG,
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR
    };

    class Logger
    {
    public:
        static Logger& Instance();

        // 로그 파일 경로 설정
        void SetLogFile(const std::string& filePath);

        // 콘솔 출력 여부 설정 (기본: true)
        void SetConsoleOutput(bool enable);

        // 기본 로그
        void Log(LogLevel level, const std::string& message);

        // 포맷 로그 (printf 스타일)
        void LogF(LogLevel level, const char* format, ...);

    private:
        Logger();
        ~Logger();

        // VS2010 호환: = delete 미지원 -> private 선언으로 복사 방지
        Logger(const Logger&);
        Logger& operator=(const Logger&);

        const char* GetLevelString(LogLevel level);
        std::string GetCurrentTime();
        void WriteInternal(LogLevel level, const std::string& message);

    private:
        CRITICAL_SECTION cs_;

        std::ofstream ofs_;
        std::string filePath_;
        bool consoleOutput_;
    };

} // namespace DVH_VAT
