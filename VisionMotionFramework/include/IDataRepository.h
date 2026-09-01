#pragma once
#include "VMF_API.h"

#include "Types.h"
#include <cstdint>
#include <string>
#include <vector>

// 확장 인터페이스 포함 (IDataRepositoryExtension 클래스 정의 필요)
#include "IDataRepositoryExtension.h"

namespace VMF 
{
    enum StorageError 
    {
        StorageSuccess = 0,
        StorageErrorWriteFailed,
        StorageErrorNotFound, 
        StorageWriteFailed,
        StorageFileNotFound,
        StorageNotFound,
        StorageGeneral,
        StorageNotImplemented = -100  // 확장 메서드 기본 반환값
    };

    /// <summary>
    /// 데이터 저장소의 핵심 인터페이스.
    /// 모든 구현체는 이 5개 메서드를 반드시 구현해야 합니다.
    /// </summary>
    class VMF_API IDataRepository
    {
    public:
        virtual ~IDataRepository() = default;

        // ── 핵심 메서드 (모든 구현체 필수) ──

        /// <summary>
        /// 기본 키-값 스타일 저장/로드
        /// </summary>
        virtual StorageError SaveParam(const std::string& recipe, const std::string& name, const std::string& value) = 0;
        virtual StorageError LoadParam(const std::string& recipe, const std::string& name, std::string& outValue) = 0;

        /// <summary>
        /// 이미지 저장: 파일에 쓰고 경로를 outPath로 반환
        /// </summary>
        virtual StorageError SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath) = 0;

        /// <summary>
        /// 초기화/종료
        /// </summary>
        virtual StorageError Initialize() = 0;
        virtual StorageError Shutdown() = 0;

        // ── 확장 메서드 (기본 구현 제공, 필요 시 오버라이드) ──
        // 상세는 IDataRepositoryExtension 참조
    };

    // IDataRepositoryExtension 클래스는 IDataRepositoryExtension.h에서 정의됨
    // (중복 정의 방지를 위해 여기서는 선언하지 않음)

} // namespace VMF
