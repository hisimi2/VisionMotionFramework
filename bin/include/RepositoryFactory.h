#pragma once

#include "IDataRepository.h"

namespace DVH_VAT
{
/*
     RepositoryFactory
     - IDataRepository 구현체(File/Sqlite 등)를 런타임에 생성하는 팩토리입니다.
     - type: "sqlite" 또는 "file"
     - config:
        * sqlite: "dbPath;imageBasePath" (imageBasePath 선택적)
        * file:   "basePath"
     - 반환: 생성된 IDataRepository* (성공 시 new로 반환, 실패 시 NULL)
       호출자는 반환된 포인터의 소유권을 가집니다(삭제 책임).
    */
    class DVH_VAT_API RepositoryFactory
    {
    public:
        static IDataRepository* CreateRepository(const std::string& type, const std::string& config);
    };

} // namespace DVH_VAT
