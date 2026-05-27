#pragma once

namespace VC
{
    struct VisionProtocolId
    {
        int stream;
        int function;

        VisionProtocolId(int s, int f)
        : stream(s), function(f)
        {
        }
    };
}


