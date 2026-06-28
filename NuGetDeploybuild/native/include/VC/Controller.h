#pragma once

#include "VisionCommAPI.h"
#include "IVisionPacket.h" 
#include "IScheduler.h"
#include "ILogger.h"
#include "ITransport.h"
#include "SECSPacket.h"

#include <string>
#include <memory>

namespace VC
{
    class SecsMessageDispatcher; // 전방 선언

    class VISION_COMM_API Controller 
    {
    public:
        Controller();
        ~Controller();

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

        SecsMessageDispatcher& GetDispatcher(); 

        // ---- 연결 키 관리 (ConnectionManager 연동) ----
        void SetConnectionKey(const std::string& key);
        std::string GetConnectionKey() const;

        /// <summary>
        /// 수신 스레드가 이미 실행 중인지 확인합니다.
        /// </summary>
        bool IsReceiving() const;

        /// <summary>
        /// 패킷 큐에 데이터가 있는지 확인합니다.
        /// </summary>
        bool HasPendingPackets() const;

    private:
        // [Pimpl 관용구]
        struct Impl;
        std::shared_ptr<Impl> m_pImpl;
    };

} // namespace VCm

