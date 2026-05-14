#pragma once
#include "VMF_API.h"
#include "Context.h"
#include <string>

namespace VMF 
{
    class IVisionEventHandler; 
    class IDataRepository;
    class IActuator;
    class SequenceBuilderBase;
    class SequenceExecutionWorker;

    class VMF_API SequenceExecutionEngine
    {
    public:
        // 생성자 선언을 소멸자보다 위에 배치하여 가독성을 높입니다.
        SequenceExecutionEngine(SequenceBuilderPtr  builder,
                            VatContextPtr       ctx,
                            VatActuatorPtr      actuator);
        
        ~SequenceExecutionEngine();

        void SetBuilder(SequenceBuilderPtr builder);
        void SetRunner(SequenceExecutionWorkerPtr runner = nullptr); // C++11: 기본값을 nullptr 지정 방식(또는 타입 생략)으로 권장

        bool RunSequence(const std::string& sequenceName);
        void StopSequence();

        DataRepositoryPtr getRepository() const;

    private:
        // 의존성 주입된 스마트 포인터들
        VatActuatorPtr          m_actuator;
        SequenceBuilderPtr      m_pBuilder;
        SequenceExecutionWorkerPtr  m_pRunner;
        VatContextPtr           m_pCtx;
    };
} // namespace VMF

