#pragma once


namespace VisionComm
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

    struct VisionProtocolId
    {
        Stream stream;
        Function function;

        VisionProtocolId(Stream s, Function f)
        : stream(s), function(f)
        {
        }
    };

    namespace VisionProtocol
    {
        static const VisionProtocolId Measure(Stream::Measure, Function::Measure);
        static const VisionProtocolId ControlRequest(Stream::Control, Function::ControlRequest);
        static const VisionProtocolId ControlAck(Stream::Control, Function::ControlAck);
        static const VisionProtocolId DeviceCheckAck(Stream::Control, Function::DeviceCheckAck);
    }
}


