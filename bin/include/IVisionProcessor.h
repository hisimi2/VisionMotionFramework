#pragma once

#include "DVH_VAT_API.h"
#include "VisionController.h"
#include "Types.h"

namespace DVH_VAT
{
    typedef std::vector<uint8_t> ByteArray;

    class IResultSink;

    enum VatCommand : int
    {
        SetCok      = 0,
        InspReady   = 1,
        Measure     = 2,
        DeviceCheck = 3,
        Light       = 4,
        Unknown     = 200
    };

    struct VisionConnectionConfig
    {
        std::string type;
        std::string address;
        int port;
        int timeoutMs;

        VisionConnectionConfig()
            : type(), address(), port(0), timeoutMs(0)
        {
        }

        VisionConnectionConfig(std::string address, int port, int timeoutMs)
            : address(address), port(port), timeoutMs(timeoutMs)
        {
        }
    };

    class DVH_VAT_API IAsyncVisionProcessor
    {
    public:
        typedef StringMap DataMap;

        virtual ~IAsyncVisionProcessor() {}

        virtual VisionCom::VisionStatus Initialize(const VisionConnectionConfig& config) = 0;
        virtual void Disconnect() = 0;
        virtual bool IsConnected() const = 0;

        virtual bool RequestSetCokAsync(const StringMap& params) = 0;
        virtual bool RequestInspReadyAsync(const StringMap& params) = 0;
        virtual bool RequestMeasureAsync(const StringMap& params) = 0;
        virtual bool RequestDeviceCheckAsync(const StringMap& params) = 0;
        virtual bool RequestLightAsync(const StringMap& params) = 0;

        virtual DataMap GetLatestData(VatCommand type) const = 0;
        virtual void ClearLatestData(VatCommand type) = 0;
        virtual bool IsValid(VatCommand type) const = 0;
        virtual bool HasReceived(VatCommand type) const = 0;
    };

    class DVH_VAT_API IVisionEventHandler : public virtual IAsyncVisionProcessor
    {
    public:
        virtual void InitializeRecvThread() = 0;

        virtual void OnSetCok(const ByteArray& body) = 0;
        virtual void OnInspReady(const ByteArray& body) = 0;
        virtual void OnMeasure(const ByteArray& body) = 0;
        virtual void OnDeviceCheck(const ByteArray& body) = 0;
        virtual void OnLight(const ByteArray& body) = 0;
    };
} // namespace DVH_VAT
