#pragma once

#include <functional> // boost::function 대신 C++ 표준 라이브러리 사용

namespace VisionCom
{
    // C++11/14: typedef 대신 가독성이 좋은 using 키워드 사용 권장
    using SchedulerTask = std::function<void()>;

    class IScheduler 
    {
    public:
        // C++11/14: 비어있는 다형성 가상 소멸자는 = default 로 구현을 명시
        virtual ~IScheduler() = default;

        virtual void Schedule(const SchedulerTask& task, int delayMs = 0) = 0;

        virtual void Start() = 0;
        virtual void Stop() = 0;
    };

} // namespace VisionCom
