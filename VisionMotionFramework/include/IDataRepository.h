#pragma once
#include "VMF_API.h"

#include "Types.h"
#include <cstdint>
#include <string>
#include <vector>

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
    /// 데이터 저장소의 확장 인터페이스.
    /// 구조화된 검사 결과 저장/로드를 위한 메서드들을 제공합니다.
    /// 기본 구현은 StorageNotImplemented를 반환하며, 필요 시 구현체에서 오버라이드합니다.
    /// </summary>
    class VMF_API IDataRepositoryExtension;

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

} // namespace VMF
