#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>

namespace VisionComm
{
    /*
    // int32를 little-endian 바이트로 추가
    inline void AppendInt32(std::vector<uint8_t>& payload, int32_t v)
    {
        payload.push_back(static_cast<uint8_t>(v & 0xFF));
        payload.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
        payload.push_back(static_cast<uint8_t>((v >> 16) & 0xFF));
        payload.push_back(static_cast<uint8_t>((v >> 24) & 0xFF));
    }

    // 바이트 배열에서 offset 위치의 little-endian int32를 읽음 (범위 검사 포함)
    inline bool TryReadInt32LE(const std::vector<uint8_t>& bytes, size_t offset, int32_t& out)
    {
        if (offset + sizeof(int32_t) > bytes.size()) {
            return false;
        }

        uint32_t v = (static_cast<uint32_t>(bytes[offset + 0]))
                   | (static_cast<uint32_t>(bytes[offset + 1]) << 8)
                   | (static_cast<uint32_t>(bytes[offset + 2]) << 16)
                   | (static_cast<uint32_t>(bytes[offset + 3]) << 24);
                   
        out = static_cast<int32_t>(v);
        return true;
    }
    */

} // namespace VisionCommm

