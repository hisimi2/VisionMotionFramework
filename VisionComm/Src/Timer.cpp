#include "StdAfx.h"
#include "Timer.h"

namespace VC{

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

} // namespace VC

