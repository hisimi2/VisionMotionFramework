#pragma once
#include <cstddef>
#include <functional> // boost::function 대신 C++ 표준 라이브러리 사용
#include <memory>     // std::unique_ptr 사용용

#include "IScheduler.h"

namespace VisionCom
{
    // C++11/14: typedef 대신 가독성이 좋은 using 키워드 사용 권장
    using WorkTask = std::function<void()>;

    class WorkerScheduler : public IScheduler
    {
    public:
        explicit WorkerScheduler(size_t threadCount = 1);
        
        // C++11/14: 다형성 클래스이므로 명시적 override 적용 권장
        ~WorkerScheduler() override;

        void Schedule(const SchedulerTask& task, int delayMs = 0) override;
        void Start() override;
        void Stop() override;

        void Enqueue(WorkTask t);   

    private:
        struct Impl;
        
        // C++11/14: 로우 포인터 대신 스마트 포인터를 사용하여 메모리 누수 원천 차단
        std::unique_ptr<Impl> m_pImpl;

        // C++11/14: 복사 생성자 및 대입 연산자는 private 선언 대신 = delete로 명시적 삭제
        WorkerScheduler(const WorkerScheduler&) = delete;
        WorkerScheduler& operator=(const WorkerScheduler&) = delete;
    };
} 
