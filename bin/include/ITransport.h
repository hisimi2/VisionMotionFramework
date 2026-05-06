#pragma once
#include "FixedLengthFramer.h"

#include <vector>
#include <cstdint>
#include <boost/function.hpp> 
#include <memory>


namespace VisionCom
{
    typedef std::vector<uint8_t> ByteVector;
    typedef boost::function<void(const ByteVector&)> TransportReceiveCallback;

    class ITransport 
    {
    public:
        virtual ~ITransport() {}

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

    typedef std::shared_ptr<ITransport> ITransportPtr; 

} // namespace VisionCom
