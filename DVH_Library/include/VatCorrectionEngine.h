#pragma once
#include "DVH_VAT_API.h"
#include "VAT_Context.h"


namespace DVH_VAT 
{
    class IVisionEventHandler; 
    class IDataRepository;
    class IVatActuator;
    class VAT_Context;
    class SequenceBuilderBase;
    class AsyncSequenceRunner;

    class DVH_VAT_API VatCorrectionEngine
    {
    public:
        ~VatCorrectionEngine();
        VatCorrectionEngine(SequenceBuilderPtr  builder,
                            VatContextPtr       ctx,
                            VatActuatorPtr      actuator);

        void SetBuilder(SequenceBuilderPtr builder);
        void SetRunner(AsyncSequenceRunnerPtr runner = AsyncSequenceRunnerPtr());

        bool RunSequence(std::string sequenceName);
        void StopSequence();

        DataRepositoryPtr getRepository() const;

    private:
        // 소유권을 엔진이 갖는 shared_ptr
        VatActuatorPtr          m_actuator;
        SequenceBuilderPtr      m_pBuilder;
        AsyncSequenceRunnerPtr  m_pRunner;
        VatContextPtr           m_pCtx;
    };
} // namespace DVH_VAT

