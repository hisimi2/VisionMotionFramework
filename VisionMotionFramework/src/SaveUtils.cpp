#include "stdafx.h"

#include "SaveUtils.h"

// Boost 관련 헤더 모두 제거하고 C++11 표준 라이브러리 사용
#include <thread>
#include <chrono>

namespace VMF 
{
    // 헤더에 선언된 함수 구현
    // (현재 SaveWithRetries 템플릿 안에서 직접 std::this_thread::sleep_for가 쓰이고 있어, 
    // 본 함수는 혹여나 다른 위치에서 외부 참조로 사용할 경우를 위해 남겨둡니다.)
    void WaitRetryDelay(int milliseconds)
    {
        if (milliseconds <= 0) 
            return;

        try {
            // C++11 표준: std::this_thread::sleep_for 사용
            std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        }
        catch (...) {
            // 스레드 인터럽트 등 예외 무시
        }
    }

} // namespace VMF
