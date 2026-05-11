#pragma once
#include <string>
#include <vector>
#include <cstdint> 
#include <functional> 
#include <memory>
#include "ITransport.h" 
#include "VisionCommAPI.h" 

namespace VisionComm
{
    using ByteArray = std::vector<uint8_t>;
    // Move-only callback: rvalue-reference parameter enforces move semantics
    using RecvCallback = std::function<void(ByteArray&&)>;

    class VISION_COMM_API VisionTcpClient : public ITransport
    {
    public:
        VisionTcpClient();
        
        // 다형성 클래스의 소멸자에 명시적 기본 처리 및 override
        ~VisionTcpClient() override;

        bool Connect(const char* ip, uint16_t port, int timeoutMs, int socketType) override;
        void Disconnect() override;
        bool IsConnected() const override;
        void StartRecvThread() override;
        int RecvOnce(ByteVector& outBytes) override;
        int Send(const ByteArray& data) override;
        void SetReceiveCallback(const RecvCallback& cb) override;

        void SetFramer(std::shared_ptr<VisionComm::IFramer> framer) override;
        void SetRecvBufferSize(size_t sz);
        void SetSocketTimeout(int sendMs, int recvMs);

        VisionTcpClient(const VisionTcpClient&) = delete;
        VisionTcpClient& operator=(const VisionTcpClient&) = delete;

    private:
        size_t m_recvBufferSize = 8192; // 기본값 추가
        std::vector<uint8_t> m_partialBuffer;
        
        // Pimpl: raw 포인터 대신 생명 주기를 관리할 스마트 포인터 사용
        struct Impl;
        std::unique_ptr<Impl> m_pImpl;
    };
} // namespace VisionCommm

