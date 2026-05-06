#pragma once

#include "IDataRepository.h"
#include <memory> // std::unique_ptr 사용을 위해 추가
#include <string>

namespace DVH_VAT
{
    /*
     RepositoryFactory
     - IDataRepository 구현체(File/Sqlite 등)를 런타임에 생성하는 팩토리입니다.
     - type: "sqlite" 또는 "file"
     - config:
        * sqlite: "dbPath;imageBasePath" (imageBasePath 선택적)
        * file:   "basePath"
     - 반환: 생성된 std::unique_ptr<IDataRepository> (성공 시 유효한 객체, 실패 시 nullptr 반환)
       스마트 포인터를 반환하여 호출자에게 소유권이 있음을 명확히 하고, 메모리 누수를 자동으로 방지합니다.
    */
    class DVH_VAT_API RepositoryFactory
    {
    public:
        // C++11/14: 리소스 소유권을 명시적으로 이전하기 위해 로우 포인터 대신 std::unique_ptr 반환
        static std::unique_ptr<IDataRepository> CreateRepository(const std::string& type, const std::string& config);
    };

} // namespace DVH_VAT
