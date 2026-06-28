#pragma once
#include "VisionProcessorBase.h"
#include "VMFComposition/VisionProtocal/VisionMemoryKeys.h"
#include <vector>
#include <string>

namespace VMF
{
    namespace VisionMemoryProtocol
    {
        enum class Stream : int
        {
            Measure = 107,
            Control = 2
        };
        enum class Function : int
        {
            Measure = 9,
            ControlRequest = 41,
            ControlAck = 42,
            DeviceCheckAck = 4
        };

        static const VC::VisionProtocolId Measure(
            static_cast<int>(Stream::Measure),
            static_cast<int>(Function::Measure));
        static const VC::VisionProtocolId ControlRequest(
            static_cast<int>(Stream::Control),
            static_cast<int>(Function::ControlRequest));
        static const VC::VisionProtocolId ControlAck(
            static_cast<int>(Stream::Control),
            static_cast<int>(Function::ControlAck));
        static const VC::VisionProtocolId DeviceCheckAck(
            static_cast<int>(Stream::Control),
            static_cast<int>(Function::DeviceCheckAck));
    }

    class VisionMemoryProcessor : public VisionProcessorBase
    {
    public:
        VisionMemoryProcessor();
        ~VisionMemoryProcessor() override;

        // 요청 — cmd로 분기
        bool RequestAsync(VisionCommand cmd,
            const StringMap& params) override;

        // 수신 — cmd + body로 통합
        void OnVisionResponse(VisionCommand cmd,
            ByteArray body) override;

        void Process() override;

    private:
        VisionMemoryProcessor(const VisionMemoryProcessor&) = delete;
        VisionMemoryProcessor& operator=(const VisionMemoryProcessor&) = delete;

        bool RequestControlAsync(const StringMap& params);
        bool RequestMeasureAsyncInternal(const StringMap& params);
        bool RequestDeviceCheckAsyncInternal(const StringMap& params);

        void HandleSetCok(ByteArray body);
        void HandleInspReady(ByteArray body);
        void HandleMeasure(ByteArray body);
        void HandleDeviceCheck(ByteArray body);

        std::vector<std::string> ParseMeasureBody(const ByteArray& body);
    };

} // namespace VMF
