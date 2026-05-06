#pragma once

#include "DVH_VAT_API.h"    // API Export 매크로
#include "IDataRepository.h" // StorageError 정의

namespace DVH_VAT 
{
    /**
     * @brief Boost 의존성을 숨기기 위한 내부 Sleep 함수 선언
     * 구현은 SaveUtils.cpp에 위치합니다.
     */
    DVH_VAT_API void WaitRetryDelay(int milliseconds);

    /**
     * SaveWithRetries: 저장을 재시도하는 유틸리티
     * 
     * [수정 내역]
     * - 템플릿 함수 특성상 헤더에 위치해야 함
     * - Boost 헤더 충돌 방지를 위해 Sleep 기능만 외부 함수(WaitRetryDelay)로 분리
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
                
                // 성공 확인 (StorageError::StorageOK 또는 0 가정)
                if (last == StorageOK) {
                    return StorageOK;
                }
            }
            catch (...) {
                // 예외 발생 시 실패로 간주하고 계속 시도
                last = StorageErrorWriteFailed;
            }

            // 마지막 시도가 아니면 대기
            if (attempt < maxRetries - 1 && retryDelayMs > 0) {
                // Boost 의존성이 있는 코드를 cpp 내부 함수 호출로 대체
                WaitRetryDelay(retryDelayMs);
            }
        }
        return last;
    }

} // namespace DVH_VAT