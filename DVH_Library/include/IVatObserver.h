#pragma once
#include "DVH_VAT_API.h"
#include <string>

namespace DVH_VAT 
{
    class DVH_VAT_API IVatObserver 
    {
    public:
        virtual ~IVatObserver() {}

        virtual void OnSequenceStarted(const std::string& id, const std::string& name) = 0;
        virtual void OnUpdateProgress(const std::string& id, int percent, const std::string& msg) = 0;
        virtual void OnError(const std::string& id, const std::string& msg) = 0;
        virtual void OnSequenceCompleted(const std::string& id, const std::string& result) = 0;
    };

    // [v100] C++11 'using' 대신 typedef 사용
    typedef IVatObserver* IVatObserverPtr;
} // namespace DVH_VAT