#pragma once

#include "IVisionPacket.h"
#include "SECSIPacket.h" // SECSPacketHeader
#include "VisionComAPI.h" // [FIX] DLL Export 매크로 포함

#include <vector>
#include <cstdint>
#include <memory> 
#include <boost/function.hpp>

// Forward declare IScheduler to avoid heavy include in header
namespace VisionCom { class IScheduler; }

namespace VisionCom
{
    typedef boost::function<void(int /*S*/, int /*F*/, const std::vector<uint8_t>& /*body*/, int /*serverIndex*/)> PacketHandler;
    typedef std::vector<uint8_t> ByteArray;

    class VISION_COM_API VisionMsgDispatcher 
    {
    public:
        explicit VisionMsgDispatcher();
        ~VisionMsgDispatcher();

        void RegisterHandler(int s, int f, PacketHandler handler);
        // [삭제] tr1 오버로드 제거됨

        void UnregisterHandler(int s, int f);
        void OnReceive(int s, int f, const std::vector<uint8_t>& body, int serverIndex);
        bool HasHandler(int s, int f);

        void Dispatch(const SECSPacketHeader& header,
            const ByteArray& body);

        void SetScheduler(std::shared_ptr<IScheduler> scheduler);

        boost::function<void(const ByteArray&)> OnSetCok;
        boost::function<void(const ByteArray&)> OnMeasure;
        boost::function<void(const ByteArray&)> OnInspReady;
        boost::function<void(const ByteArray&)> OnDeviceCheck;
        boost::function<void(const ByteArray&)> OnLight;

    private:
        struct Impl;
        Impl* m_pImpl;
    };

} // namespace VisionCom
