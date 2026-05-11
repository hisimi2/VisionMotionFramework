#include "StdAfx.h"
#include "SECSPacket.h"
#include <cstring>
#include <cstdint>

namespace VisionComm
{
    SECSPacket::SECSPacket()
    {
    }

    ByteArray SECSPacket::ToByteArray() const
    {
        int totalLen = static_cast<int>(sizeof(SECSPacketHeader) + m_body.size());

        SECSPacketHeader headerCopy = m_header;
        headerCopy.nLength = totalLen;

        ByteArray packet;
        packet.reserve(static_cast<size_t>(totalLen));
        packet.resize(sizeof(SECSPacketHeader));

        if (!packet.empty()) {
            std::memcpy(packet.data(), &headerCopy, sizeof(SECSPacketHeader));
        }

        if (!m_body.empty()) {
            packet.insert(packet.end(), m_body.begin(), m_body.end());
        }

        return packet;
    }

    bool SECSPacket::FromByteArray(const ByteArray& raw)
    {
        if (raw.size() < sizeof(SECSPacketHeader))
            return false;

        std::memcpy(&m_header, raw.data(), sizeof(SECSPacketHeader));

        if (static_cast<int>(raw.size()) < m_header.nLength)
            return false;

        const auto bodySize = raw.size() - sizeof(SECSPacketHeader);
        if (bodySize >0)
        {
            m_body.assign(raw.begin() + sizeof(SECSPacketHeader), raw.end());
        }
        else
        {
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

    void SECSPacket::SetProtocol(Stream stream, Function function)
    {
        m_header.nS = static_cast<int>(stream);
        m_header.nF = static_cast<int>(function);
    }

    void SECSPacket::SetProtocol(const VisionProtocolId& protocolId)
    {
        SetProtocol(protocolId.stream, protocolId.function);
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
        return false;
    }

} // namespace VisionCommm

