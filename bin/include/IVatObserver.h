#pragma once
#include "VMF_API.h"
#include <string>

namespace VMF 
{
    class VMF_API IVatObserver 
    {
    public:
        virtual ~IVatObserver() = default;

        virtual void OnSequenceStarted(const std::string& id, const std::string& name) = 0;
        virtual void OnUpdateProgress(const std::string& id, int percent, const std::string& msg) = 0;
        virtual void OnError(const std::string& id, const std::string& msg) = 0;
        virtual void OnSequenceCompleted(const std::string& id, const std::string& result) = 0;
    };

    using IVatObserverPtr = IVatObserver*;
} // namespace VMF
