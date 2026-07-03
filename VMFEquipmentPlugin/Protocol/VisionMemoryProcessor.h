#pragma once
#include "VisionProcessorBase.h"
#include "VisionMemoryKeys.h"
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

        // NuGet 버전 API: 단일 RequestAsync 메서드로 통합
        bool RequestAsync(VisionCommand cmd, const StringMap& params) override;

        // NuGet 버전 API: 단일 OnVisionResponse 메서드로 통합
        void OnVisionResponse(VisionCommand cmd, ByteArray body) override;

        void Process() override;

    private:
        VisionMemoryProcessor(const VisionMemoryProcessor&) = delete;
        VisionMemoryProcessor& operator=(const VisionMemoryProcessor&) = delete;

        std::vector<std::string> ParseMeasureBody(const ByteArray& body);
    };

} // namespace VMF
