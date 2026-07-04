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

        // IVisionProcessor 인터페이스: 개별 명령 메서드
        bool RequestSetCokAsync(const StringMap& params) override;
        bool RequestInspReadyAsync(const StringMap& params) override;
        bool RequestMeasureAsync(const StringMap& params) override;
        bool RequestDeviceCheckAsync(const StringMap& params) override;
        bool RequestLightAsync(const StringMap& params) override;

        // IVisionProcessor 인터페이스: 개별 응답 메서드
        void OnSetCok(ByteArray body) override;
        void OnInspReady(ByteArray body) override;
        void OnMeasure(ByteArray body) override;
        void OnDeviceCheck(ByteArray body) override;
        void OnLight(ByteArray body) override;

        void Process() override;

    private:
        VisionMemoryProcessor(const VisionMemoryProcessor&) = delete;
        VisionMemoryProcessor& operator=(const VisionMemoryProcessor&) = delete;

        /// <summary>
        /// Control 명령(SetCok, InspReady) 전송 공통 헬퍼
        /// </summary>
        bool SendControlRequest(const StringMap& params, VisionCommand cmd);

        std::vector<std::string> ParseMeasureBody(const ByteArray& body);
    };

} // namespace VMF
