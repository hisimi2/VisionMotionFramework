#pragma once
#include "VMF_API.h"
#include "Context.h"
#include <string>

namespace VMF 
{
    class IVisionProcessor; 
    class IDataRepository;
    class IActuator;
    class SequenceBuilderBase;
    class AsyncExecutor;

    class VMF_API RunController
    {
    public:
        // 생성자 선언을 소멸자보다 위에 배치하여 가독성을 높입니다.
    RunController(SequenceBuilderPtr  builder,
                      VisionContextPtr       ctx,
                      VisionActuatorPtr      actuator);
        
        ~RunController();

        void SetBuilder(SequenceBuilderPtr builder);
        void SetRunner(AsyncExecutorPtr runner = nullptr); 

        bool RunSequence(const std::string& sequenceName);
        void StopSequence();

DataRepositoryPtr GetRepository() const;
        VisionContextPtr GetContext() const;

    private:
        // 의존성 주입된 스마트 포인터들
        VisionActuatorPtr          m_actuator;
        SequenceBuilderPtr      m_pBuilder;
        AsyncExecutorPtr        m_pRunner;
        VisionContextPtr           m_pCtx;
    };
} // namespace VMF

