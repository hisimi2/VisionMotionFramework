#pragma once
#include "EC_API.h"
#include "ITask.h"
#include <string>

namespace EC 
{
    class Context;

    // ISequence: sequence 실행 추상 인터페이스
    class ISequence 
    {
    public:
        virtual ~ISequence() = default;

        virtual bool Execute(Context& context) = 0;
        virtual void Abort() = 0;
        virtual std::string GetSequenceName() const = 0;
        virtual std::string GetTaskName() const = 0;
        virtual void AddTask(TaskPtr step) = 0;
    };

    using SequencePtr = std::unique_ptr<ISequence>;
} 
