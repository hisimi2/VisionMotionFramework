#pragma once

#include "VisionCommAPI.h"
#include "IVisionPacket.h" 
#include "IScheduler.h"
#include "ILogger.h"
#include "ITransport.h"
#include "IFramer.h"
#include "FixedLengthFramer.h"
#include "VisionTcpClient.h"
#include "SECSIPacket.h"

#include <vector>
#include <string>
#include <memory>         

namespace VisionComm
{
    class VisionMsgDispatcher; // 전방 선언

    class VISION_COMM_API VisionController 
    {
    public:
        VisionController();
        ~VisionController();

        void SetTransport(std::shared_ptr<ITransport> transport);
        void SetLogger(std::shared_ptr<ILogger> logger);
        void SetScheduler(std::shared_ptr<IScheduler> scheduler);

        bool Initialize(const char* szIp, int nPort, int nSocketType, int timeoutMs);
        void Disconnect();
        bool IsConnected() const;

        // 비동기 전송
        Status SendPacketAsync(const SECSPacket& pkt);

        void StartReceiving();
        void StopReceiving();
		void PacketThread();

        VisionMsgDispatcher& GetDispatcher(); 

    private:
        // [Pimpl 관용구]
        struct Impl;
        std::shared_ptr<Impl> m_pImpl;
    };

} // namespace VisionCommm

