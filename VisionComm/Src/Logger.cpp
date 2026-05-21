#include "StdAfx.h"
#include "Logger.h"

#include <iostream>
#include <ctime>
#include <sstream>

namespace VisionComm
{

    static std::string CurrentTimeString()
    {
        std::time_t t = std::time(nullptr);
        char buf[64] = {0};
        std::tm tm;
#ifdef WIN32
        localtime_s(&tm, &t);
#else
        tm = *std::localtime(&t);
#endif
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
        return std::string(buf);
    }

    Logger::Logger()
        : m_mutex(),
          m_ofs(),
          m_useFile(false)
    {
    }

    Logger::Logger(const std::string& logFilePath)
        : m_mutex(),
          m_ofs(),
          m_useFile(false)
    {
        SetLogFile(logFilePath);
    }

    Logger::~Logger()
    {
        // boost::unique_lock -> std::unique_lock 또는 std::lock_guard
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_ofs.is_open()) {
            m_ofs.flush();
            m_ofs.close();
        }
    }

    void Logger::SetLogFile(const std::string& path)
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_ofs.is_open()) {
            m_ofs.flush();
            m_ofs.close();
        }
        m_ofs.open(path.c_str(), std::ios::out | std::ios::app);
        m_useFile = m_ofs.is_open();
    }

    void Logger::WriteLine(const std::string& msg)
    {
        // assume caller holds no lock; but we take lock here for thread-safety
        std::lock_guard<std::mutex> lk(m_mutex);
        const std::string line = CurrentTimeString() + " " + msg + "\n";
        if (m_useFile && m_ofs.is_open()) {
            m_ofs << line;
            m_ofs.flush();
        } else {
            // fallback to console
            std::cout << line;
        }
    }

    void Logger::Log(const std::string& message)
    {
        try {
            WriteLine(message);
        } catch (...) {
            // swallow exceptions to avoid affecting runtime
        }
    }

    void Logger::LogDebug(const std::string& message)
    {
        try {
            WriteLine(std::string("[DEBUG] ") + message);
        } catch (...) {
            // swallow
        }
    }

    // 기존 코드 호환을 위한 SaveLog 구현(기존 호출을 Log로 위임)
    void Logger::SaveLog(const std::string& message)
    {
        Log(message);
    }

    void Logger::SaveLog(const char* message)
    {
        if (message) Log(std::string(message));
    }

    // 3-인자 오버로드: tag + message + level -> 하나의 문자열로 결합하여 기록
    void Logger::SaveLog(const std::string& tag, const std::string& message, int level)
    {
        try {
            std::ostringstream ss;
            ss << "[" << tag << "] ";
            ss << "(level=" << level << ") ";
            ss << message;
            WriteLine(ss.str());
        } catch (...) {
            // swallow
        }
    }

    void Logger::SaveLog(const char* tag, const char* message, int level)
    {
        try {
            std::ostringstream ss;
            if (tag) ss << "[" << tag << "] ";
            ss << "(level=" << level << ") ";
            if (message) ss << message;
            WriteLine(ss.str());
        } catch (...) {
            // swallow
        }
    }

    // 3-인자 오버로드: tag + message + opt(string) -> 하나의 문자열로 결합하여 기록
    void Logger::SaveLog(const std::string& tag, const std::string& message, const std::string& opt)
    {
        try {
            std::ostringstream ss;
            ss << "[" << tag << "] ";
            if (!opt.empty()) {
                ss << "(" << opt << ") ";
            }
            ss << message;
            WriteLine(ss.str());
        } catch (...) {
            // swallow
        }
    }

    void Logger::SaveLog(const char* tag, const char* message, const char* opt)
    {
        try {
            std::ostringstream ss;
            if (tag) ss << "[" << tag << "] ";
            if (opt && opt[0] != '\0') ss << "(" << opt << ") ";
            if (message) ss << message;
            WriteLine(ss.str());
        } catch (...) {
            // swallow
        }
    }

} // namespace VisionCommm

