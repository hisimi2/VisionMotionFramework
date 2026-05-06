#pragma once
#include "FixedLengthFramer.h"

#include <vector>
#include <cstdint>
#include <functional> // boost::function 대신 C++ 표준 라이브러리 사용
#include <memory>

namespace VisionCom
{
    // C++11/14: typedef 대신 가독성이 좋은 using 키워드 사용 권장
    using ByteVector = std::vector<uint8_t>;
    using TransportReceiveCallback = std::function<void(const ByteVector&)>;

    class ITransport 
    {
    public:
        // C++11/14: 비어있는 다형성 가상 소멸자는 = default 로 구현을 명시
        virtual ~ITransport() = default;

        // 연결: true 성공
        virtual bool Connect(const char* ip, uint16_t port, int timeoutMs, int socketType) = 0;
        virtual void Disconnect() = 0;

        // 동기 전송: 보낸 바이트 수 반환(실패 시 음수)
        virtual int Send(const ByteVector& data) = 0;

        virtual void StartRecvThread() = 0;

        // 동기/테스트용 수신: 0 이상 바이트 수, 음수 오류
        virtual int RecvOnce(ByteVector& out) = 0;

        // 현재 연결 상태 질의
        virtual bool IsConnected() const = 0;

        // 비동기 콜백 등록(선택적)
        virtual void SetReceiveCallback(const TransportReceiveCallback& cb) = 0;

        virtual void SetFramer(std::shared_ptr<VisionCom::IFramer> framer) = 0;
    };

    using ITransportPtr = std::shared_ptr<ITransport>; 

} // namespace VisionCom
