#pragma once
#include "IVisionPacket.h"
#include "VisionCommAPI.h"
#include "VisionProtocol.h"
#include <vector>

namespace VC
{
    struct SECSPacketHeader 
    {
        int nMsgId = 0;   
        int nS = 0;
        int nF = 0;
        int nLength = 0;
        
        SECSPacketHeader() = default; 
    };

    class VISION_COMM_API SECSPacket : public IVisionPacket
    {
    public:
        SECSPacket();
        ~SECSPacket() override = default;
        
        // IVisionPacket 인터페이스 구현
        ByteArray ToByteArray() const override;
        bool FromByteArray(const ByteArray& raw) override;

        int GetOpCode() const override;       // Stream
        void SetOpCode(int code) override;
        
        int GetSubCode() const override;      // Function
        void SetSubCode(int code) override;

        void SetProtocol(int stream, int function);
        void SetProtocol(const VisionProtocolId& protocolId);
        
        uint32_t GetCorrelationId() const override; 
        void SetCorrelationId(uint32_t id) override;

        const ByteArray& GetBody() const override;
        void SetBody(const ByteArray& body) override;

        size_t GetHeaderSize() const override;

        bool ParseResponse(int& dataId, int& nRet, std::vector<std::string>& results) const override;

    private:
        SECSPacketHeader m_header;
        ByteArray m_body;
    };

} // namespace VCm

