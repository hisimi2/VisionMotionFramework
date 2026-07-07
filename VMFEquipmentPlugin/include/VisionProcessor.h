#pragma once
#include "VisionProcessorBase.h"
#include "VisionParamKeys.h"
#include <vector>
#include <string>

namespace VMF
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

extern const VC::VisionProtocolId Protocol_Measure;
    extern const VC::VisionProtocolId Protocol_ControlRequest;
    extern const VC::VisionProtocolId Protocol_ControlAck;
    extern const VC::VisionProtocolId Protocol_DeviceCheckAck;


    class VisionProcessor : public VisionProcessorBase
    {
    public:
        VisionProcessor();
        ~VisionProcessor() override;

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
        VisionProcessor(const VisionProcessor&) = delete;
        VisionProcessor& operator=(const VisionProcessor&) = delete;

        /// <summary>
        /// Control 명령(SetCok, InspReady) 전송 공통 헬퍼
        /// </summary>
        bool SendControlRequest(const StringMap& params, VisionCommand cmd);

        std::vector<std::string> ParseMeasureBody(const ByteArray& body);
    };

} // namespace VMF
