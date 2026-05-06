#pragma once
#include "IVisionPacket.h"
#include "VisionComAPI.h"
#include <vector>

namespace VisionCom
{

    struct SECSPacketHeader 
    {
        int nMsgId;
        int nS;
        int nF;
        int nLength;
        SECSPacketHeader() : nMsgId(0), nS(0), nF(0), nLength(0) {}
    };

    class VISION_COM_API SECSPacket : public IVisionPacket {
    public:
        SECSPacket();
        virtual ~SECSPacket();
        
        // IVisionPacket 인터페이스 구현
        virtual ByteArray ToByteArray() const override;
        virtual bool FromByteArray(const ByteArray& raw) override;

        virtual int GetOpCode() const override;       // Stream
        virtual void SetOpCode(int code) override;
        
        virtual int GetSubCode() const override;      // Function
        virtual void SetSubCode(int code) override;

        virtual uint32_t GetCorrelationId() const override; // MsgId
        virtual void SetCorrelationId(uint32_t id) override;

        virtual const ByteArray& GetBody() const override;
        virtual void SetBody(const ByteArray& body) override;

        virtual size_t GetHeaderSize() const override;

        virtual bool ParseResponse(int& dataId, int& nRet, std::vector<std::string>& results) const override;

    private:
        // [수정] VisionPacket m_pkt 제거하고 직접 데이터 멤버 관리
        SECSPacketHeader m_header;
        ByteArray m_body;
    };

} // namespace VisionCom