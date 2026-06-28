#pragma once
#include <string>
#include <vector>
#include <cstdint> 
#include <functional> 
#include <memory>
#include "ITransport.h"
#include "VisionCommAPI.h" 

namespace VC
{
    class TcpClient : public ITransport
    {
    public:
        TcpClient();
        ~TcpClient() override;

        bool Connect(const char* ip, uint16_t port, int timeoutMs, int socketType) override;
        void Disconnect() override;
        bool IsConnected() const override;
        void StartRecvThread() override;
        int RecvOnce(ByteVector& outBytes) override;
        int Send(const ByteVector& data) override;
        void SetReceiveCallback(const TransportReceiveCallback& cb) override;

        void SetFramer(std::shared_ptr<IFramer> framer) override;
        void SetRecvBufferSize(size_t sz);
        void SetSocketTimeout(int sendMs, int recvMs);

        TcpClient(const TcpClient&) = delete;
        TcpClient& operator=(const TcpClient&) = delete;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_pImpl;
    };
}

