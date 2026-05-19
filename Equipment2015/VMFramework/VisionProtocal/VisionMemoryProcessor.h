#pragma once

#include "VisionProcessorBase.h"
#include "DVH_VAT/VisionProtocal/VisionMemoryKeys.h"

#include <vector>
#include <string>

namespace VMF
{
    // "VMF는 '어떻게(How) 보낼지'만 정의하고, '무엇을(What/Which Keys) 보낼지'는 Equipment 프로젝트가 결정해야 합니다."
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

        static const VisionComm::VisionProtocolId Measure(static_cast<int>(Stream::Measure), static_cast<int>(Function::Measure));
        static const VisionComm::VisionProtocolId ControlRequest(static_cast<int>(Stream::Control), static_cast<int>(Function::ControlRequest));
        static const VisionComm::VisionProtocolId ControlAck(static_cast<int>(Stream::Control), static_cast<int>(Function::ControlAck));
        static const VisionComm::VisionProtocolId DeviceCheckAck(static_cast<int>(Stream::Control), static_cast<int>(Function::DeviceCheckAck));
    }

    class VisionMemoryProcessor : public VisionProcessorBase
    {
    public:
        VisionMemoryProcessor();
        ~VisionMemoryProcessor() override;

        bool RequestSetCokAsync(const StringMap& params) override;
        bool RequestInspReadyAsync(const StringMap& params) override;
        bool RequestMeasureAsync(const StringMap& params) override;
        bool RequestDeviceCheckAsync(const StringMap& params) override;
        bool RequestLightAsync(const StringMap& params) override;

        // 변경: 바디를 by-value로 받아 호출자가 std::move로 소유권을 전달할 수 있도록 함
        void OnSetCok(ByteArray body) override;
        void OnInspReady(ByteArray body) override;
        void OnMeasure(ByteArray body) override;
        void OnDeviceCheck(ByteArray body) override;
        void OnLight(ByteArray body) override;

        void Process() override;

    private:
        VisionMemoryProcessor(const VisionMemoryProcessor&) = delete;
        VisionMemoryProcessor& operator=(const VisionMemoryProcessor&) = delete;

        std::vector<std::string> ParseMeasureBody(const ByteArray& body);
    };
}
