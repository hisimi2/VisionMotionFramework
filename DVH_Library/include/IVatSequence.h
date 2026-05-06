#pragma once
#include "DVH_VAT_API.h"
#include "ITask.h"
#include <string>

namespace DVH_VAT 
{
    class VAT_Context;
    class IVatActuator;
    class IDataRepository;
    class IResultSink;

    // IVatSequence: sequence 실행 추상 인터페이스
    class DVH_VAT_API IVatSequence 
    {
    public:
        // C++11/14: 다형성 객체의 명시적 default 소멸자 적용
        virtual ~IVatSequence() = default;

        virtual bool Execute(VAT_Context& context, IVatActuator* actuator) = 0;
        virtual void Abort() = 0;
        virtual std::string GetSequenceName() const = 0;
        virtual std::string GetTaskName() const = 0;
        virtual void AddTask(TaskStepPtr step) = 0;
    };
} // namespace DVH_VAT
