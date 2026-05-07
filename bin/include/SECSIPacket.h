#pragma once
#include "IVisionPacket.h"
#include "VisionComAPI.h"
#include "VisionProtocol.h"
#include <vector>

namespace VisionCom
{

    struct SECSPacketHeader 
    {
        int nMsgId = 0;   // C++11 멤버 이니셜라이저 사용
        int nS = 0;
        int nF = 0;
        int nLength = 0;
        
        // 생성자 본문을 비우고 컴파일러 기본 생성자 위임
        SECSPacketHeader() = default; 
    };

    class VISION_COM_API SECSPacket : public IVisionPacket {
    public:
        SECSPacket();
        
        // C++11/14: 다형성 클래스이므로 명시적 override 및 default 소멸자 적용 권장
        ~SECSPacket() override = default;
        
        // IVisionPacket 인터페이스 구현
        ByteArray ToByteArray() const override;
        bool FromByteArray(const ByteArray& raw) override;

        int GetOpCode() const override;       // Stream
        void SetOpCode(int code) override;
        
        int GetSubCode() const override;      // Function
        void SetSubCode(int code) override;

        void SetProtocol(VisionStream stream, VisionFunction function);
        void SetProtocol(const VisionProtocolId& protocolId);
        
        uint32_t GetCorrelationId() const override; // MsgId
        void SetCorrelationId(uint32_t id) override;

        const ByteArray& GetBody() const override;
        void SetBody(const ByteArray& body) override;

        size_t GetHeaderSize() const override;

        bool ParseResponse(int& dataId, int& nRet, std::vector<std::string>& results) const override;

    private:
        // 직접 데이터 멤버 관리
        SECSPacketHeader m_header;
        ByteArray m_body;
    };

} // namespace VisionCom
