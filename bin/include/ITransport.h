#pragma once
#include "FixedLengthFramer.h"

#include <vector>
#include <cstdint>
#include <functional> 
#include <memory>

namespace VC
{
    using ByteVector = std::vector<uint8_t>;
    using TransportReceiveCallback = std::function<void(ByteVector&&)>;
    

    class ITransport
    {
    public:
        virtual ~ITransport() = default;

        virtual bool Connect(const char* ip, uint16_t port, int timeoutMs, int socketType) = 0;
        virtual void Disconnect() = 0;
        virtual int Send(const ByteVector& data) = 0;
        virtual void StartRecvThread() = 0;
        virtual int RecvOnce(ByteVector& out) = 0;
        virtual bool IsConnected() const = 0;
        virtual void SetReceiveCallback(const TransportReceiveCallback& cb) = 0;
        virtual void SetFramer(std::shared_ptr<IFramer> framer) = 0;
    };

    using ITransportPtr = std::shared_ptr<ITransport>;

} // namespace VCm

