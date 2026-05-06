#pragma once

#include "VisionComAPI.h"
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

namespace VisionCom
{
    class VisionMsgDispatcher; // 전방 선언

    class VISION_COM_API VisionController 
    {
    public:
        VisionController();
        ~VisionController();

        void SetTransport(std::shared_ptr<ITransport> transport);
        void SetLogger(std::shared_ptr<ILogger> logger);
        void SetScheduler(std::shared_ptr<IScheduler> scheduler);

        bool Initialize(char* szIp, int nPort, int nSocketType, int timeoutMs);
        void Disconnect();
        bool IsConnected() const;

        // 비동기 전송
        VisionStatus SendPacketAsync(const SECSPacket& pkt);

        void StartReceiving();
        void StopReceiving();
		void PacketThread();

        VisionMsgDispatcher& GetDispatcher(); 

    private:
        // [Pimpl 관용구]
        // C++11/14: 로우 포인터 대신 std::unique_ptr 사용 권장
        struct Impl;
        std::unique_ptr<Impl> m_pImpl;
    };

} // namespace VisionCom
