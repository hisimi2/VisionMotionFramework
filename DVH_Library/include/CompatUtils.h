#pragma once

// VS2010 친화적 전처리: Windows 헤더 최소화 및 min/max 매크로 방지
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

// [v100] 표준 C++ 헤더
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <memory>    // std::auto_ptr 등 (v100)
#include <cassert>

// 새로 구현한 Logger 포함
#include "Logger.h"

namespace DVH_VAT 
{
    inline void DBG(const char* fn) {
        std::ostringstream ss;
        ss << fn << " tid=" << ::GetCurrentThreadId() << "\n";
        ::OutputDebugStringA(ss.str().c_str());
    }

    inline void DBG_FMT(const char* fn, const char* msg) {
        std::ostringstream ss;
        ss << fn << " tid=" << ::GetCurrentThreadId() << " : " << msg << "\n";
        ::OutputDebugStringA(ss.str().c_str());
    }

    static std::string makeLogPrefix(const std::string& seqName)
    {
        std::ostringstream oss;
        oss << "[VatSequence:" << seqName << "] ";
        return oss.str();
    }


    // [유틸리티] std::to_string 미지원 (v100) -> 헬퍼 함수 구현
    template <typename T>
    inline std::string ToString(const T& value) 
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    // 특수화가 필요한 경우 추가 (예: float 정밀도)
    inline std::string ToString(double value) 
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    // 공정 로그를 위한 단순 출력 헬퍼 -> Logger 연결
    inline void LogTask(const std::string& msg) 
    {
        // Logger를 통해 파일/콘솔 동시 출력
        Logger::Instance().Log(LOG_INFO, msg);
    }

    // (옵션) 에러 로그 헬퍼 추가
    inline void LogError(const std::string& msg)
    {
        Logger::Instance().Log(LOG_ERROR, msg);
    }

} // namespace DVH_VAT
