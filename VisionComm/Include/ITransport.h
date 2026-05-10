#pragma once
#include "FixedLengthFramer.h"

#include <vector>
#include <cstdint>
#include <functional> // boost::function ???C++ ?쒖? ?쇱씠釉뚮윭由??ъ슜
#include <memory>

namespace VisionComm
{
    using ByteVector = std::vector<uint8_t>;

    // Move-only callback: rvalue-reference parameter enforces move semantics
    using TransportReceiveCallback = std::function<void(ByteVector&&)>;

    class ITransport
    {
    public:
        virtual ~ITransport() = default;

        // ?곌껐: true ?깃났
        virtual bool Connect(const char* ip, uint16_t port, int timeoutMs, int socketType) = 0;
        virtual void Disconnect() = 0;

        // ?숆린 ?꾩넚: 蹂대궦 諛붿씠????諛섑솚(?ㅽ뙣 ???뚯닔)
        virtual int Send(const ByteVector& data) = 0;

        virtual void StartRecvThread() = 0;

        // ?숆린/?뚯뒪?몄슜 ?섏떊: 0 ?댁긽 諛붿씠???? ?뚯닔 ?ㅻ쪟
        virtual int RecvOnce(ByteVector& out) = 0;

        // ?꾩옱 ?곌껐 ?곹깭 吏덉쓽
        virtual bool IsConnected() const = 0;

        // 鍮꾨룞湲?肄쒕갚 ?깅줉(?좏깮??
        virtual void SetReceiveCallback(const TransportReceiveCallback& cb) = 0;

        virtual void SetFramer(std::shared_ptr<VisionComm::IFramer> framer) = 0;
    };

    using ITransportPtr = std::shared_ptr<ITransport>; 

} // namespace VisionCommm

