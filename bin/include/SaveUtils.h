#pragma once

#include "VMF_API.h"    // API Export 매크로
#include "IDataRepository.h" // StorageError 정의

#include <thread>
#include <chrono>

namespace VMF 
{
    /**
     * SaveWithRetries: 저장을 재시도하는 유틸리티
     * 
     * [수정 내역]
     * - C++11 이후 표준 쓰레드 지연 사용: std::this_thread::sleep_for
     * - 외부 구현 파일(CPP) 분리 제거 -> 헤더 단독 파일 (Header-only)
     * - IDataRepository.h 의 StorageError 반환값 'StorageSuccess' 사용 
     */
    template <typename SaveFunc>
    StorageError SaveWithRetries(SaveFunc saveFunc, int maxRetries, int retryDelayMs = 100)
    {
        StorageError last = StorageErrorWriteFailed;
        
        if (maxRetries <= 0) maxRetries = 1;

        for (int attempt = 0; attempt < maxRetries; ++attempt) {
            try {
                // 저장 시도
                last = saveFunc();
                
                // 성공 확인 (StorageSuccess 로 매칭)
                if (last == StorageSuccess) {
                    return StorageSuccess;
                }
            }
            catch (...) {
                // 예외 발생 시 실패로 간주하고 계속 시도
                last = StorageErrorWriteFailed;
            }

            // 마지막 시도가 아니면 대기
            if (attempt < maxRetries - 1 && retryDelayMs > 0) {
                // C++14: 표준 스레드 지연 함수 사용
                std::this_thread::sleep_for(std::chrono::milliseconds(retryDelayMs));
            }
        }
        return last;
    }

} // namespace VMF
