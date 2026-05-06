#pragma once
#include <string>
#include <vector>
#include <cstdint> // <stdint.h> 대신 권장되는 헤더 사용
#include <functional> // boost::function 대신 C++ 표준 라이브러리 사용
#include <memory>
#include "ITransport.h" 
#include "VisionComAPI.h" 

namespace VisionCom
{
    // C++11/14: typedef 대신 가독성이 좋은 using 키워드 사용 권장
    using ByteArray = std::vector<uint8_t>;
    using RecvCallback = std::function<void(const ByteArray&)>;

    class VISION_COM_API VisionTcpClient : public ITransport
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

        void SetFramer(std::shared_ptr<VisionCom::IFramer> framer) override;
        void SetRecvBufferSize(size_t sz);
        void SetSocketTimeout(int sendMs, int recvMs);

        // C++11/14: 복사 생성자 및 할당 연산자를 private에 선언하는 대신
        // delete 키워드를 사용해 더 명확하게 복사 불가를 선언
        VisionTcpClient(const VisionTcpClient&) = delete;
        VisionTcpClient& operator=(const VisionTcpClient&) = delete;

    private:
        size_t m_recvBufferSize = 8192; // 기본값 추가
        std::vector<uint8_t> m_partialBuffer;
        
        // Pimpl: raw 포인터 대신 생명 주기를 관리할 스마트 포인터 사용
        struct Impl;
        std::unique_ptr<Impl> m_pImpl;
    };
} // namespace VisionCom
