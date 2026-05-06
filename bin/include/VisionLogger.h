#pragma once

#include "ILogger.h"

#include <string>
#include <fstream>
#include <mutex> // boost::mutex 대신 C++ 표준 라이브러리 사용

namespace VisionCom
{
    class VisionLogger : public ILogger 
    {
    public:
        VisionLogger();
        explicit VisionLogger(const std::string& logFilePath);
        
        // C++11/14: 다형성 클래스이므로 명시적 override 적용 권장
        ~VisionLogger() override;

        // ILogger 구현
        void Log(const std::string& message) override;
        void LogDebug(const std::string& message) override;

        // 기존 코드 호환성: SaveLog 멤버 추가
        void SaveLog(const std::string& message);
        void SaveLog(const char* message);

        // 3-인자 오버로드 (기존 호출과 호환) - 여러 시그니처 제공
        void SaveLog(const std::string& tag, const std::string& message, int level);
        void SaveLog(const char* tag, const char* message, int level);

        // 추가: 세 번째 인수가 문자열인 기존 호출과 호환
        void SaveLog(const std::string& tag, const std::string& message, const std::string& opt);
        void SaveLog(const char* tag, const char* message, const char* opt);

        // 추가 유틸: 파일 경로 설정(런타임)
        void SetLogFile(const std::string& path);

    private:
        void WriteLine(const std::string& msg);

        // boost::mutex -> std::mutex 교체
        std::mutex m_mutex;
        std::ofstream m_ofs;
        bool m_useFile;
    };

} // namespace VisionCom
