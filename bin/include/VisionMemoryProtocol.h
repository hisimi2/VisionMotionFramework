#pragma once

#include "VisionProtocol.h"

namespace VMF
{
    namespace VisionMemoryProtocol
    {
        enum class Stream : int
        {
            Measure =   107,
            Control =   2
        };

        enum class Function : int
        {
            Measure         = 9,
            ControlRequest  = 41,
            ControlAck      = 42,
            DeviceCheckAck  = 4
        };

        static const VisionComm::VisionProtocolId Measure(static_cast<int>(Stream::Measure), static_cast<int>(Function::Measure));
        static const VisionComm::VisionProtocolId ControlRequest(static_cast<int>(Stream::Control), static_cast<int>(Function::ControlRequest));
        static const VisionComm::VisionProtocolId ControlAck(static_cast<int>(Stream::Control), static_cast<int>(Function::ControlAck));
        static const VisionComm::VisionProtocolId DeviceCheckAck(static_cast<int>(Stream::Control), static_cast<int>(Function::DeviceCheckAck));
    }
}
