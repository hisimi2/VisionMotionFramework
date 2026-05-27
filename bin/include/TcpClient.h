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
    using ByteArray = std::vector<uint8_t>;
    using RecvCallback = std::function<void(ByteArray&&)>;

    class TcpClient : public ITransport
    {
    public:
        TcpClient();
        
        // 다형성 클래스의 소멸자에 명시적 기본 처리 및 override
        ~TcpClient() override;

        bool Connect(const char* ip, uint16_t port, int timeoutMs, int socketType) override;
        void Disconnect() override;
        bool IsConnected() const override;
        void StartRecvThread() override;
        int RecvOnce(ByteVector& outBytes) override;
        int Send(const ByteArray& data) override;
        void SetReceiveCallback(const RecvCallback& cb) override;

        void SetFramer(std::shared_ptr<IFramer> framer) override;
        void SetRecvBufferSize(size_t sz);
        void SetSocketTimeout(int sendMs, int recvMs);

        TcpClient(const TcpClient&) = delete;
        TcpClient& operator=(const TcpClient&) = delete;

    private:
        size_t m_recvBufferSize = 8192; // 기본값 추가
        std::vector<uint8_t> m_partialBuffer;
        
        // Pimpl: raw 포인터 대신 생명 주기를 관리할 스마트 포인터 사용
        struct Impl;
        std::unique_ptr<Impl> m_pImpl;
    };
} // namespace VCm

