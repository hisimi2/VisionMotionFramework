#include "stdafx.h"

#include "SaveUtils.h"

// Boost 헤더는 구현부에서만 포함
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace DVH_VAT 
{
    // 헤더에 선언된 함수 구현
    void WaitRetryDelay(int milliseconds)
    {
        if (milliseconds <= 0) 
            return;

        try {
            // v100 환경: boost::this_thread 사용
            boost::this_thread::sleep(boost::posix_time::milliseconds(milliseconds));
        }
        catch (...) {
            // 스레드 인터럽트 등 예외 무시
        }
    }

} // namespace DVH_VAT