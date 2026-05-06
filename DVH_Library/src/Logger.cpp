#include "StdAfx.h"
#include "Logger.h"
#include <iostream>
#include <ctime>
#include <cstdio>
#include <vector>

namespace DVH_VAT
{
    // 싱글톤 인스턴스
    Logger& Logger::Instance()
    {
        static Logger instance;
        return instance;
    }

    Logger::Logger() 
        : consoleOutput_(true) 
    {
        ::InitializeCriticalSection(&cs_);
    }

    Logger::~Logger()
    {
        if (ofs_.is_open())
        {
            ofs_.close();
        }

        ::DeleteCriticalSection(&cs_);
    }

    void Logger::SetLogFile(const std::string& filePath)
    {
        ::EnterCriticalSection(&cs_);
        
        if (ofs_.is_open()) {
            ofs_.close();
        }

        filePath_ = filePath;
        // append 모드로 열기
        ofs_.open(filePath_.c_str(), std::ios::out | std::ios::app);

        if (!ofs_)
        {
            std::cerr << "[Logger] Failed to open log file: " << filePath_ << std::endl;
        }

        ::LeaveCriticalSection(&cs_);
    }

    void Logger::SetConsoleOutput(bool enable)
    {
        ::EnterCriticalSection(&cs_);
        consoleOutput_ = enable;
        ::LeaveCriticalSection(&cs_);
    }

    const char* Logger::GetLevelString(LogLevel level)
    {
        switch (level) {
        case LOG_DEBUG: return "DEBUG";
        case LOG_INFO:  return "INFO";
        case LOG_WARN:  return "WARN";
        case LOG_ERROR: return "ERROR";
        default:        return "UNKNOWN";
        }
    }

    std::string Logger::GetCurrentTime()
    {
        std::time_t t = std::time(NULL);
        char buf[64] = { 0 };
        std::tm tm_storage;

        // MSVC / MinGW: localtime_s 사용 (반환값 errno_t)
        if (localtime_s(&tm_storage, &t) == 0) {
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_storage);
            return std::string(buf);
        }
        return "";
    }

    void Logger::Log(LogLevel level, const std::string& message)
    {
        WriteInternal(level, message);
    }

    void Logger::LogF(LogLevel level, const char* format, ...)
    {
        if (!format) return;

        char buffer[2048];
        va_list args;
        va_start(args, format);

        _vsnprintf_s(buffer, _countof(buffer), _TRUNCATE, format, args);

        va_end(args);

        WriteInternal(level, std::string(buffer));
    }

    void Logger::WriteInternal(LogLevel level, const std::string& message) {
        ::EnterCriticalSection(&cs_);

        std::string timeStr = GetCurrentTime();
        const char* levelStr = GetLevelString(level);

        // 포맷: [2023-10-01 12:00:00] [INFO] 메시지 내용
        if (consoleOutput_)
        {
            std::cout << "[" << timeStr << "] [" << levelStr << "] " << message << std::endl;
        }

        if (ofs_.is_open())
        {
            ofs_ << "[" << timeStr << "] [" << levelStr << "] " << message << std::endl;
        }

        ::LeaveCriticalSection(&cs_);
    }

} // namespace DVH_VAT
