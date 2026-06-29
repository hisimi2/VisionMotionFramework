#pragma once

#include <string>
#include <fstream>
#include <cstdarg>
#include <mutex> // Win32 CRITICAL_SECTION 대신 C++11 표준 라이브러리 사용

namespace VMF
{
    // 기존 시스템과의 호환성을 위해 전통적 enum 유지
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

    /// <summary>
    /// LogF에서 사용하는 내부 버퍼의 기본 크기 (바이트)
    /// </summary>
    static const int LOG_BUFFER_SIZE = 2048;

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

    private:
        Logger();
        ~Logger();

        const char* GetLevelString(LogLevel level);
        std::string GetCurrentTime();
        void WriteInternal(LogLevel level, const std::string& message);

    private:
        // CRITICAL_SECTION 대체
        std::mutex mutex_;

        std::ofstream ofs_;
        std::string filePath_;
        bool consoleOutput_;
    };

} // namespace VMF
