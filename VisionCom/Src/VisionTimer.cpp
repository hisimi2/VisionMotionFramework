#include "StdAfx.h"
#include "VisionTimer.h"

#include <chrono> // std::chrono 사용을 위한 포함

namespace VisionCom{

VisionTimer::VisionTimer()
{
    Start();
}

// C++14: 명시적 기본 소멸자 사용
VisionTimer::~VisionTimer() = default;

void VisionTimer::Start()
{
    m_start = std::chrono::steady_clock::now();
}

double VisionTimer::ElapsedSeconds() const
{
    // C++14: auto를 통한 타입 추론 적용
    const auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = now - m_start;
    return diff.count();
}

long VisionTimer::ElapsedMillis() const
{
    const auto now = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start);
    return static_cast<long>(ms.count());
}

} // namespace VisionCom
