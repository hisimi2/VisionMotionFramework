#pragma once

#include <boost/chrono.hpp>

namespace VisionCom
{

    class VisionTimer 
    {
    public:
        VisionTimer();
        ~VisionTimer();

        void Start();
        // Returns elapsed seconds as double (fractional)
        double ElapsedSeconds() const;
        // Returns elapsed milliseconds
        long ElapsedMillis() const;

    private:
        boost::chrono::steady_clock::time_point m_start;
    };

} // namespace VisionCom