#pragma once
#include "DVH_VAT_API.h"
#include "VAT_Context.h"
#include <string>

namespace DVH_VAT 
{
    class IVisionEventHandler; 
    class IDataRepository;
    class IVatActuator;
    class SequenceBuilderBase;
    class AsyncSequenceRunner;

    class DVH_VAT_API VatCorrectionEngine
    {
    public:
        // 생성자 선언을 소멸자보다 위에 배치하여 가독성을 높입니다.
        VatCorrectionEngine(SequenceBuilderPtr  builder,
                            VatContextPtr       ctx,
                            VatActuatorPtr      actuator);
        
        ~VatCorrectionEngine();

        void SetBuilder(SequenceBuilderPtr builder);
        void SetRunner(AsyncSequenceRunnerPtr runner = nullptr); // C++11: 기본값을 nullptr 지정 방식(또는 타입 생략)으로 권장

        bool RunSequence(const std::string& sequenceName);
        void StopSequence();

        DataRepositoryPtr getRepository() const;

    private:
        // 의존성 주입된 스마트 포인터들
        VatActuatorPtr          m_actuator;
        SequenceBuilderPtr      m_pBuilder;
        AsyncSequenceRunnerPtr  m_pRunner;
        VatContextPtr           m_pCtx;
    };
} // namespace DVH_VAT

