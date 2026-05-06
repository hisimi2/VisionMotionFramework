#include "StdAfx.h"
#include "VisionTimer.h"

namespace VisionCom{

VisionTimer::VisionTimer()
{
    Start();
}

VisionTimer::~VisionTimer()
{
}

void VisionTimer::Start()
{
    m_start = boost::chrono::steady_clock::now();
}

double VisionTimer::ElapsedSeconds() const
{
    boost::chrono::steady_clock::time_point now = boost::chrono::steady_clock::now();
    boost::chrono::duration<double> diff = now - m_start;
    return diff.count();
}

long VisionTimer::ElapsedMillis() const
{
    boost::chrono::steady_clock::time_point now = boost::chrono::steady_clock::now();
    boost::chrono::milliseconds ms = boost::chrono::duration_cast<boost::chrono::milliseconds>(now - m_start);
    return static_cast<long>(ms.count());
}

} // namespace VisionCom