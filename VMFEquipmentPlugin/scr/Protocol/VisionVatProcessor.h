#pragma once

#include "visioncommunicationmanager.h"
#include "VMFEquipmentPluginExport.h"
#include "ivisionclient.h"

namespace VMF_PLUGIN
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

class VisionVatProcessor : public VMF::VisionCommunicationManager
    {
    public:
        VisionVatProcessor();
        ~VisionVatProcessor() override;

		bool RequestSetCokAsync(const VMF::StringMap& params) override;
		bool RequestInspReadyAsync(const VMF::StringMap& params) override;
		bool RequestMeasureAsync(const VMF::StringMap& params) override;
		bool RequestDeviceCheckAsync(const VMF::StringMap& params) override;
		bool RequestLightAsync(const VMF::StringMap& params) override;

		// 변경: 바디를 by-value로 받아 호출자가 std::move로 소유권을 전달할 수 있도록 함
		void OnSetCok(VMF::ByteArray body) override;
		void OnInspReady(VMF::ByteArray body) override;
		void OnMeasure(VMF::ByteArray body) override;
		void OnDeviceCheck(VMF::ByteArray body) override;
		void OnLight(VMF::ByteArray body) override;

        void Process() override;

private:
        VisionVatProcessor(const VisionVatProcessor&) = delete;
        VisionVatProcessor& operator=(const VisionVatProcessor&) = delete;

        std::vector<std::string> ParseMeasureBody(const VMF::ByteArray& body);
    };

} // namespace VMF_PLUGIN
