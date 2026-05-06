#pragma once
#include <string>
#include <vector>
#include <stdint.h>
#include <boost/function.hpp> 
#include "ITransport.h" 
#include "VisionComAPI.h" 

namespace VisionCom
{
    typedef std::vector<uint8_t> ByteArray;
    typedef boost::function<void(const ByteArray&)> RecvCallback;

    class VisionTcpClient : public ITransport
    {
    public:
        VisionTcpClient();
        virtual ~VisionTcpClient();

        virtual bool Connect(const char* ip, uint16_t port, int timeoutMs, int socketType) override;
        virtual void Disconnect() override;
        virtual bool IsConnected() const override;
		virtual void StartRecvThread() override;
        virtual int RecvOnce(ByteVector& outBytes) override;
        virtual int Send(const ByteArray& data) override;
        virtual void SetReceiveCallback(const RecvCallback& cb) override;

		virtual void SetFramer(std::shared_ptr<VisionCom::IFramer> framer) override;
        void SetRecvBufferSize(size_t sz);
        void SetSocketTimeout(int sendMs, int recvMs);

    private:
        VisionTcpClient(const VisionTcpClient&);
        VisionTcpClient& operator=(const VisionTcpClient&) { return *this; }
		size_t m_recvBufferSize;
		std::vector<uint8_t> m_partialBuffer;
        struct Impl;
        Impl* m_pImpl;
    };
} 
