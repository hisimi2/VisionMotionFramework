#pragma once

#include "VMF_API.h"
#include "VisionProcessorBase.h"
#include "VisionMemoryKeys.h"

#include <vector>
#include <string>

namespace VMF
{
    class VMF_API VisionMemoryProcessor : public VisionProcessorBase
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
