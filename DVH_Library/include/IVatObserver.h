#pragma once
#include "DVH_VAT_API.h"
#include <string>

namespace DVH_VAT 
{
    class DVH_VAT_API IVatObserver 
    {
    public:
        // C++11/14: 다형성 클래스의 가상 소멸자에 명시적인 = default 적용
        virtual ~IVatObserver() = default;

        virtual void OnSequenceStarted(const std::string& id, const std::string& name) = 0;
        virtual void OnUpdateProgress(const std::string& id, int percent, const std::string& msg) = 0;
        virtual void OnError(const std::string& id, const std::string& msg) = 0;
        virtual void OnSequenceCompleted(const std::string& id, const std::string& result) = 0;
    };

    // C++11/14: typedef 대신 가독성이 높은 using 키워드 사용 권장
    using IVatObserverPtr = IVatObserver*;
} // namespace DVH_VAT
