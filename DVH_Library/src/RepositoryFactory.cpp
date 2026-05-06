#include "StdAfx.h"
#include "RepositoryFactory.h"
#include "SqliteDataRepository.h"
// 명시적 상대 경로로 정확한 구현 헤더를 포함
#include "FileDataRepository.h"
#include <algorithm>
#include <cctype>
#include <iostream>

namespace DVH_VAT {

// [v100] 문자열 Trim 함수
static std::string trim(const std::string& s)
{
    if (s.empty()) return s;
    
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) {
        start++;
    }
    
    size_t end = s.size();
    while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) {
        end--;
    }
    
    return s.substr(start, end - start);
}

// [v100] std::transform용 헬퍼 함수 (int(*)(int) 캐스팅 대신 사용)
static int ToLowerChar(int c) {
    return std::tolower(c);
}

IDataRepository* RepositoryFactory::CreateRepository(const std::string& type, const std::string& config)
{
    std::string t = type;
    std::transform(t.begin(), t.end(), t.begin(), ToLowerChar);

    if (t == "sqlite") {
        // config format: "dbPath;imageBasePath"
        std::string dbPath;
        std::string imageBase;
        size_t sep = config.find_first_of(';');
        
        if (sep == std::string::npos) {
            dbPath = trim(config);
            imageBase.clear();
        } else {
            dbPath = trim(config.substr(0, sep));
            imageBase = trim(config.substr(sep + 1));
        }

        if (dbPath.empty()) return NULL;

        try {
            // [주의] SqliteDataRepository 가 구현되어 있지 않거나 링크되지 않으면 에러 가능
            // 현재 코드 구조상 구현체가 있다고 가정
            SqliteDataRepository* repo = new SqliteDataRepository(dbPath, imageBase);
            return static_cast<IDataRepository*>(repo);
        } catch (...) {
            return NULL;
        }
    }
    else if (t == "file" || t == "filesystem") {
        std::string basePath = trim(config);
        if (basePath.empty()) return NULL;
        
        try {
            FileDataRepository* repo = new FileDataRepository(basePath);
            return static_cast<IDataRepository*>(repo);
        } catch (...) {
            return NULL;
        }
    }

    return NULL;
}

} // namespace DVH_VAT