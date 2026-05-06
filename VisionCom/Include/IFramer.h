#pragma once

#include <vector>


namespace VisionCom
{
    typedef std::vector<uint8_t> ByteVector;

    class IFramer 
    {
    public:
        virtual ~IFramer() {}

        // 바이트를 공급
        virtual void FeedBytes(const ByteVector& bytes) = 0;

        // 가능한 경우 다음 프레임을 꺼내어 frame에 채우고 true 반환,
        // 없으면 false 반환
        virtual bool NextFrame(ByteVector& frame) = 0;
    };

    typedef std::shared_ptr<IFramer> IFramerPtr;

} // namespace VisionCom