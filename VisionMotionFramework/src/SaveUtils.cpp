#include "stdafx.h"

#include "SaveUtils.h"


#include <thread>
#include <chrono>

namespace VMF 
{
    void WaitRetryDelay(int milliseconds)
    {
        if (milliseconds <= 0) 
            return;

        try
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        }
        catch (...) {
            // 스레드 인터럽트 등 예외 무시
        }
    }

} // namespace VMF
