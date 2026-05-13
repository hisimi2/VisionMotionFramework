#pragma once

#include "VMF_API.h"
#include "Controller.h"
#include "Types.h"

#include <vector>
#include <cstdint>
#include <string>

namespace VMF
{
    /// "VMF는 '어떻게(How) 보낼지'만 정의하고, '무엇을(What/Which Keys) 보낼지'는 Equipment 프로젝트가 결정

    using ByteArray = std::vector<uint8_t>;

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
            : type(""), address(""), port(0), timeoutMs(0)
        {
        }

        VisionConnectionConfig(std::string address, int port, int timeoutMs)
            : type(""), address(std::move(address)), port(port), timeoutMs(timeoutMs) // C++11 std::move ?ъ슜 理쒖쟻??
        {
        }
    };

    class VMF_API IAsyncVisionProcessor
    {
    public:
        using DataMap = StringMap;
        virtual ~IAsyncVisionProcessor() = default;
        virtual VisionComm::Status Initialize(const VisionConnectionConfig& config) = 0;
        virtual void Disconnect() = 0;
        virtual bool IsConnected() const = 0;
        virtual bool RequestSetCokAsync(const StringMap& params)        = 0;
        virtual bool RequestInspReadyAsync(const StringMap& params)     = 0;
        virtual bool RequestMeasureAsync(const StringMap& params)       = 0;
        virtual bool RequestDeviceCheckAsync(const StringMap& params)   = 0;
        virtual bool RequestLightAsync(const StringMap& params)         = 0;
        virtual DataMap GetLatestData(VatCommand type) const            = 0;
        virtual void ClearLatestData(VatCommand type)                   = 0;
        virtual bool IsValid(VatCommand type) const                     = 0;
        virtual bool HasReceived(VatCommand type) const                 = 0;
    };

    class VMF_API IVisionEventHandler : public virtual IAsyncVisionProcessor
    {
    public:
        ~IVisionEventHandler() override = default;

        virtual void InitializeRecvThread()         = 0;
        virtual void OnSetCok(ByteArray body)       = 0;
        virtual void OnInspReady(ByteArray body)    = 0;
        virtual void OnMeasure(ByteArray body)      = 0;
        virtual void OnDeviceCheck(ByteArray body)  = 0;
        virtual void OnLight(ByteArray body)        = 0;
    };
} // namespace VMF

