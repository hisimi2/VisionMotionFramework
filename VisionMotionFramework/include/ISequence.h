#pragma once
#include "VMF_API.h"
#include "ITask.h"
#include <string>

namespace VMF 
{
    class Context;
    class IActuator;
    class IDataRepository;
    class IResultSink;

    // ISequence: sequence 실행 추상 인터페이스
    class VMF_API ISequence 
    {
    public:
        virtual ~ISequence() = default;

        virtual bool Execute(Context& context, IActuator* actuator) = 0;
        virtual void Abort() = 0;
        virtual std::string GetSequenceName() const = 0;
        virtual std::string GetTaskName() const = 0;
        virtual void AddTask(TaskStepPtr step) = 0;
    };
} // namespace VMF
