#pragma once

#include "IVisionPacket.h"
#include "SECSIPacket.h" // SECSPacketHeader
#include "VisionComAPI.h" // [FIX] DLL Export 매크로 포함

#include <vector>
#include <cstdint>
#include <memory> 
#include <functional> // boost::function 대신 C++ 표준 라이브러리 사용

// Forward declare IScheduler to avoid heavy include in header
namespace VisionCom { class IScheduler; }

namespace VisionCom
{
    // C++11/14: typedef 대신 가독성이 좋은 using 키워드 사용 권장
    using PacketHandler = std::function<void(int /*S*/, int /*F*/, const std::vector<uint8_t>& /*body*/, int /*serverIndex*/)>;
    using ByteArray = std::vector<uint8_t>;

    class VISION_COM_API VisionMsgDispatcher 
    {
    public:
        explicit VisionMsgDispatcher();
        ~VisionMsgDispatcher();

        void RegisterHandler(int s, int f, PacketHandler handler);

        void UnregisterHandler(int s, int f);
        void OnReceive(int s, int f, const std::vector<uint8_t>& body, int serverIndex);
        bool HasHandler(int s, int f);

        void Dispatch(const SECSPacketHeader& header,
            const ByteArray& body);

        void SetScheduler(std::shared_ptr<IScheduler> scheduler);

        // boost::function -> std::function 교체
        std::function<void(const ByteArray&)> OnSetCok;
        std::function<void(const ByteArray&)> OnMeasure;
        std::function<void(const ByteArray&)> OnInspReady;
        std::function<void(const ByteArray&)> OnDeviceCheck;
        std::function<void(const ByteArray&)> OnLight;

    private:
        struct Impl;
        
        // C++11/14: 로우 포인터 대신 스마트 포인터를 사용하여 메모리 누수 방지(Pimpl 관용구)
        std::unique_ptr<Impl> m_pImpl;
    };

} // namespace VisionCom
