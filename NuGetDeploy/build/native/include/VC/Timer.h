#pragma once

#include <chrono>

namespace VC
{
    class Timer 
    {
    public:
        Timer();
        ~Timer();

        void Start();
        double ElapsedSeconds() const;
        long ElapsedMillis() const;

    private:
        std::chrono::steady_clock::time_point m_start;
    };

} // namespace VC

