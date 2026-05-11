#include "StdAfx.h"
#include "Timer.h"

#include <chrono> // std::chrono 사용을 위한 포함

namespace VisionComm{

Timer::Timer()
{
    Start();
}


Timer::~Timer() = default;

void Timer::Start()
{
    m_start = std::chrono::steady_clock::now();
}

double Timer::ElapsedSeconds() const
{
    
    const auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = now - m_start;
    return diff.count();
}

long Timer::ElapsedMillis() const
{
    const auto now = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start);
    return static_cast<long>(ms.count());
}

} // namespace VisionCommm

