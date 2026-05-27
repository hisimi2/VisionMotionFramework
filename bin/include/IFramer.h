#pragma once

#include <vector>
#include <memory>  // std::shared_ptr, std::unique_ptr 사용을 위한 추가
#include <cstdint> // uint8_t 사용을 위한 추가

namespace VC
{
    using ByteVector = std::vector<uint8_t>;

    class IFramer 
    {
    public:
        virtual ~IFramer() = default;

        // 바이트를 공급
        virtual void FeedBytes(const ByteVector& bytes) = 0;

        // 가능한 경우 다음 프레임을 반환하는 소유 포인터를 반환한다.
        // 반환값이 nullptr이면 아직 프레임이 완성되지 않았음을 의미한다.
        // 반환된 포인터가 가리키는 벡터는 0바이트일 수 있으며, 이는 "빈(0바이트) 프레임"을 나타낸다.
        virtual std::unique_ptr<ByteVector> NextFrame() = 0;
    };

    using IFramerPtr = std::shared_ptr<IFramer>;

} // namespace VCm

