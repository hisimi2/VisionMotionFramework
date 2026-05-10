#pragma once

#include "DVH_VAT_API.h"
#include "VisionProcessorBase.h"

#include <vector>
#include <string>

namespace DVH_VAT
{
    class DVH_VAT_API VisionSLTProcessor : public VisionProcessorBase
    {
    public:
        VisionSLTProcessor();
        ~VisionSLTProcessor() override;

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
        VisionSLTProcessor(const VisionSLTProcessor&) = delete;
        VisionSLTProcessor& operator=(const VisionSLTProcessor&) = delete;

        std::vector<std::string> ParseMeasureBody(const ByteArray& body);
    };
} // namespace DVH_VAT
