#pragma once

#include <chrono> // boost::chrono 대신 C++ 표준 라이브러리 사용

namespace VisionCom
{

    class VisionTimer 
    {
    public:
        VisionTimer();
        ~VisionTimer(); // 구현부(.cpp)에서 = default 로 처리됨

        void Start();
        // Returns elapsed seconds as double (fractional)
        double ElapsedSeconds() const;
        // Returns elapsed milliseconds
        long ElapsedMillis() const;

    private:
        // boost::chrono -> std::chrono 로 변경
        std::chrono::steady_clock::time_point m_start;
    };

} // namespace VisionCom
