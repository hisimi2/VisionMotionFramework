#pragma once

#include "ILogger.h"

#include <string>
#include <fstream>
#include <mutex>

namespace VC
{
    class Logger : public ILogger 
    {
    public:
        Logger();
        explicit Logger(const std::string& logFilePath);
        ~Logger() override;

        // ILogger 구현
        void Log(const std::string& message) override;
        void LogDebug(const std::string& message) override;

        // 기존 코드 호환성: SaveLog — 단일 메시지
        void SaveLog(const std::string& message);
        void SaveLog(const char* message);

        // 3-인자 오버로드 (기존 호출과 호환)
        void SaveLog(const std::string& tag, const std::string& message, int level);
        void SaveLog(const char* tag, const char* message, int level);
        void SaveLog(const std::string& tag, const std::string& message, const std::string& opt);
        void SaveLog(const char* tag, const char* message, const char* opt);

        // 추가 유틸: 파일 경로 설정(런타임)
        void SetLogFile(const std::string& path);

    private:
        void WriteLine(const std::string& msg);

        std::mutex m_mutex;
        std::ofstream m_ofs;
        bool m_useFile;
    };

} // namespace VCm

