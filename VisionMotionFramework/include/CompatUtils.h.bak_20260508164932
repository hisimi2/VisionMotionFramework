#pragma once

// Windows 헤더 최소화 및 min/max 매크로 방지
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h> // OutputDebugStringA 사용을 위한 명시적 포함
#endif

// C++ 표준 헤더
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <memory>    // std::unique_ptr, std::shared_ptr 등
#include <cassert>
#include <thread>    // std::this_thread::get_id() 확보

// 새로 구현한 Logger 포함
#include "Logger.h"

namespace DVH_VAT 
{
    inline void DBG(const char* fn) {
        std::ostringstream ss;
        // C++11: Windows 전용 ::GetCurrentThreadId() 대신 표준 라이브러리 사용
        ss << fn << " tid=" << std::this_thread::get_id() << "\n";
#if defined(_WIN32)
        ::OutputDebugStringA(ss.str().c_str());
#else
        std::cout << ss.str();
#endif
    }

    inline void DBG_FMT(const char* fn, const char* msg) {
        std::ostringstream ss;
        ss << fn << " tid=" << std::this_thread::get_id() << " : " << msg << "\n";
#if defined(_WIN32)
        ::OutputDebugStringA(ss.str().c_str());
#else
        std::cout << ss.str();
#endif
    }

    // 헤더 구현 시 중복 심볼 생성을 막기 위해 static -> inline 변경
    inline std::string makeLogPrefix(const std::string& seqName)
    {
        std::ostringstream oss;
        oss << "[VatSequence:" << seqName << "] ";
        return oss.str();
    }

    // [유틸리티] 사용자 정의 클래스 문자열 변환용 헬퍼 
    // (기본 자료형은 C++11 std::to_string 사용을 권장)
    template <typename T>
    inline std::string ToString(const T& value) 
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    // 특수화 포맷 유지를 위한 double 오버로드
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
