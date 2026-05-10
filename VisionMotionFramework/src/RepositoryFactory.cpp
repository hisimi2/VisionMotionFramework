#include "StdAfx.h"
#include "RepositoryFactory.h"
#include "SqliteDataRepository.h"
// 명시적 상대 경로로 정확한 구현 헤더를 포함
#include "FileDataRepository.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>

namespace DVH_VAT {

// [v100] 문자열 Trim 함수
static std::string trim(const std::string& s)
{
    if (s.empty()) return s;
    
    // C++11 이후 권장되는 반복자(iterator) 기반 탐색
    auto start = s.begin();
    while (start != s.end() && std::isspace(static_cast<unsigned char>(*start))) {
        start++;
    }
    
    auto end = s.end();
    while (end > start && std::isspace(static_cast<unsigned char>(*(end - 1)))) {
        end--;
    }
    
    return std::string(start, end);
}

// std::transform용 헬퍼 함수
static int ToLowerChar(int c) {
    return std::tolower(c);
}

std::unique_ptr<IDataRepository> RepositoryFactory::CreateRepository(const std::string& type, const std::string& config)
{
    std::string t = type;
    std::transform(t.begin(), t.end(), t.begin(), ToLowerChar);

    if (t == "sqlite") {
        // config format: "dbPath;imageBasePath"
        std::string dbPath;
        std::string imageBase;
        // C++11: 타입 추론 auto 활용
        auto sep = config.find_first_of(';');
        
        if (sep == std::string::npos) {
            dbPath = trim(config);
            imageBase.clear();
        } else {
            dbPath = trim(config.substr(0, sep));
            imageBase = trim(config.substr(sep + 1));
        }

        // NULL을 C++11 표준인 nullptr로 대체
        if (dbPath.empty()) return nullptr;

        try {
            // C++14: std::make_unique를 사용하여 안전하게 할당 및 리턴 (캐스팅 불필요)
            return std::make_unique<SqliteDataRepository>(dbPath, imageBase);
        } catch (...) {
            return nullptr;
        }
    }
    else if (t == "file" || t == "filesystem") {
        std::string basePath = trim(config);
        if (basePath.empty()) return nullptr;
        
        try {
            // C++14: std::make_unique 활용
            return std::make_unique<FileDataRepository>(basePath);
        } catch (...) {
            return nullptr;
        }
    }

    return nullptr;
}

} // namespace DVH_VAT
