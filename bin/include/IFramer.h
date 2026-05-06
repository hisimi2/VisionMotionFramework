#pragma once

#include <vector>
#include <memory>  // std::shared_ptr 사용을 위한 추가
#include <cstdint> // uint8_t 사용을 위한 추가

namespace VisionCom
{
    // C++11/14: typedef 대신 가독성이 좋은 using 키워드 사용 권장
    using ByteVector = std::vector<uint8_t>;

    class IFramer 
    {
    public:
        // C++11/14: 비어있는 다형성 가상 소멸자는 = default 로 구현을 명시
        virtual ~IFramer() = default;

        // 바이트를 공급
        virtual void FeedBytes(const ByteVector& bytes) = 0;

        // 가능한 경우 다음 프레임을 꺼내어 frame에 채우고 true 반환,
        // 없으면 false 반환
        virtual bool NextFrame(ByteVector& frame) = 0;
    };

    // C++11/14: typedef 대신 using 키워드 사용 분리 선언
    using IFramerPtr = std::shared_ptr<IFramer>;

} // namespace VisionCom
