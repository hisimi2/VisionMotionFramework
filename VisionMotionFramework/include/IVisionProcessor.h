#pragma once

#include "DVH_VAT_API.h"
#include "VisionController.h"
#include "Types.h"

#include <vector>
#include <cstdint>
#include <string>

namespace DVH_VAT
{
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

        // C++11 硫ㅻ쾭 ?대땲?쒕씪?댁?瑜??ъ슜??寃쎌슦 ?앹꽦?먭? 媛꾧껐?댁쭛?덈떎.
        VisionConnectionConfig()
            : type(""), address(""), port(0), timeoutMs(0)
        {
        }

        VisionConnectionConfig(std::string address, int port, int timeoutMs)
            : type(""), address(std::move(address)), port(port), timeoutMs(timeoutMs) // C++11 std::move ?ъ슜 理쒖쟻??
        {
        }
    };

    class DVH_VAT_API IAsyncVisionProcessor
    {
    public:
        using DataMap = StringMap;

        virtual ~IAsyncVisionProcessor() = default;

        virtual VisionComm::VisionStatus Initialize(const VisionConnectionConfig& config) = 0;
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

    // 媛???곸냽(virtual inheritance) - ?명꽣?섏씠??蹂묓빀???꾪빐 ?ъ슜
    class DVH_VAT_API IVisionEventHandler : public virtual IAsyncVisionProcessor
    {
    public:
        ~IVisionEventHandler() override = default;

        virtual void InitializeRecvThread() = 0;

        // 蹂寃? 諛붾뵒瑜?by-value濡?諛쏆븘 ?몄텧?먭? std::move濡??뚯쑀沅뚯쓣 ?댁쟾?????덈룄濡??덉슜
        virtual void OnSetCok(ByteArray body) = 0;
        virtual void OnInspReady(ByteArray body) = 0;
        virtual void OnMeasure(ByteArray body) = 0;
        virtual void OnDeviceCheck(ByteArray body) = 0;
        virtual void OnLight(ByteArray body) = 0;
    };
} // namespace DVH_VAT

