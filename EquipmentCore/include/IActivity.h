#pragma once
#include "EC_API.h"
#include "ITask.h"
#include <string>

namespace EC 
{
    class Context;

    // IActivity: Activity 실행 추상 인터페이스
    class IActivity 
    {
    public:
        virtual ~IActivity() = default;

        virtual bool Execute(Context& context) = 0;
        virtual void Abort() = 0;
        virtual std::string GetActivityName() const = 0;
        virtual void AddTask(TaskPtr step) = 0;
    };

    using ActivityPtr = std::unique_ptr<IActivity>;
}
