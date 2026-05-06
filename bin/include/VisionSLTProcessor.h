#pragma once

#include "DVH_VAT_API.h"
#include "VisionProcessorBase.h"

namespace DVH_VAT
{
    class DVH_VAT_API VisionSLTProcessor : public VisionProcessorBase
    {
    public:
        VisionSLTProcessor();
        virtual ~VisionSLTProcessor();

        virtual bool RequestSetCokAsync(const StringMap& params);
        virtual bool RequestInspReadyAsync(const StringMap& params);
        virtual bool RequestMeasureAsync(const StringMap& params);
        virtual bool RequestDeviceCheckAsync(const StringMap& params);
        virtual bool RequestLightAsync(const StringMap& params);

        virtual void OnSetCok(const ByteArray& body);
        virtual void OnInspReady(const ByteArray& body);
        virtual void OnMeasure(const ByteArray& body);
        virtual void OnDeviceCheck(const ByteArray& body);
        virtual void OnLight(const ByteArray& body);

        virtual void Process();

    private:
        VisionSLTProcessor(const VisionSLTProcessor&);
        VisionSLTProcessor& operator=(const VisionSLTProcessor&);

        std::vector<std::string> ParseMeasureBody(const ByteArray& body);
    };
} // namespace DVH_VAT
