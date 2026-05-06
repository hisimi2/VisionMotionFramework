#include "StdAfx.h"
#include "SECSIPacket.h"
#include <cstring> // memcpy
#include <cstdint>

namespace VisionCom
{
    SECSPacket::SECSPacket() 
    {
        // m_header는 생성자에서 0으로 초기화됨
    }

    SECSPacket::~SECSPacket() 
    {
    }

    ByteArray SECSPacket::ToByteArray() const 
    {
        // 전체 패킷 크기 계산 (int로 관리되는 헤더 필드와 호환되도록 int 사용)
        int totalLen = static_cast<int>(sizeof(SECSPacketHeader) + m_body.size());

        // 헤더의 nLength 필드 업데이트 (로컬 복사본 사용)
        SECSPacketHeader headerCopy = m_header;
        headerCopy.nLength = totalLen;

        ByteArray packet;
        // 필요한 용량 확보
        packet.reserve(static_cast<size_t>(totalLen));
        // 헤더 공간 확보
        packet.resize(sizeof(SECSPacketHeader));

        // 헤더 복사: VS2010 호환성 위해 data() 대신 &packet[0] 사용
        if (!packet.empty()) {
            std::memcpy(&packet[0], &headerCopy, sizeof(SECSPacketHeader));
        }

        // 바디 복사
        if (!m_body.empty()) {
            packet.insert(packet.end(), m_body.begin(), m_body.end());
        }

        return packet;
    }

    bool SECSPacket::FromByteArray(const ByteArray& raw) 
    {
        if (raw.size() < sizeof(SECSPacketHeader))
            return false;

        // 헤더 파싱
        std::memcpy(&m_header, raw.data(), sizeof(SECSPacketHeader));

        // 길이 검증 (선택 사항)
        if (static_cast<int>(raw.size()) < m_header.nLength)
            return false;

        // 바디 파싱
        size_t bodySize = raw.size() - sizeof(SECSPacketHeader);
        if (bodySize > 0) {
            m_body.assign(raw.begin() + sizeof(SECSPacketHeader), raw.end());
        } else {
            m_body.clear();
        }

        return true;
    }

    int SECSPacket::GetOpCode() const 
    {
        return m_header.nS;
    }

    void SECSPacket::SetOpCode(int code) 
    {
        m_header.nS = code;
    }

    int SECSPacket::GetSubCode() const 
    {
        return m_header.nF;
    }

    void SECSPacket::SetSubCode(int code) 
    {
        m_header.nF = code;
    }

    uint32_t SECSPacket::GetCorrelationId() const 
    {
        return static_cast<uint32_t>(m_header.nMsgId);
    }

    void SECSPacket::SetCorrelationId(uint32_t id) 
    {
        m_header.nMsgId = static_cast<int>(id);
    }

    const ByteArray& SECSPacket::GetBody() const 
    {
        return m_body;
    }

    void SECSPacket::SetBody(const ByteArray& body) 
    {
        m_body = body;
        m_header.nLength = static_cast<int>(sizeof(SECSPacketHeader) + m_body.size());
    }

    size_t SECSPacket::GetHeaderSize() const 
    {
        return sizeof(SECSPacketHeader);
    }

    bool SECSPacket::ParseResponse(int& dataId, int& nRet, std::vector<std::string>& results) const 
    {
        // 기본 구현은 false (필요 시 Packet_S2F42 등에서 오버라이드)
        return false;
    }

} // namespace VisionCom
