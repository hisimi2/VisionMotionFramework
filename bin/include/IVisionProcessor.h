#pragma once

#include "DVH_VAT_API.h"
#include "VisionController.h"
#include "Types.h"

#include <vector>
#include <cstdint>
#include <string>

namespace DVH_VAT
{
    // C++11/14: typedef 대신 사용하여 가독성 향상
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

        // C++11 멤버 이니셜라이저를 사용할 경우 생성자가 간결해집니다.
        VisionConnectionConfig()
            : type(""), address(""), port(0), timeoutMs(0)
        {
        }

        VisionConnectionConfig(std::string address, int port, int timeoutMs)
            : type(""), address(std::move(address)), port(port), timeoutMs(timeoutMs) // C++11 std::move 사용 최적화
        {
        }
    };

    class DVH_VAT_API IAsyncVisionProcessor
    {
    public:
        // C++11/14: typedef 대신 using 예약어 활용
        using DataMap = StringMap;

        // C++11/14: 비어있는 가상 소멸자를 명시적으로 default화
        virtual ~IAsyncVisionProcessor() = default;

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

    // 가상 상속(virtual inheritance) - 인터페이스 병합을 위해 사용
    class DVH_VAT_API IVisionEventHandler : public virtual IAsyncVisionProcessor
    {
    public:
        // C++11/14: 부모가 소멸자를 가지고 있지만, 인터페이스에 명시적으로 기본 정의
        ~IVisionEventHandler() override = default;

        virtual void InitializeRecvThread() = 0;

        virtual void OnSetCok(const ByteArray& body) = 0;
        virtual void OnInspReady(const ByteArray& body) = 0;
        virtual void OnMeasure(const ByteArray& body) = 0;
        virtual void OnDeviceCheck(const ByteArray& body) = 0;
        virtual void OnLight(const ByteArray& body) = 0;
    };
} // namespace DVH_VAT
