#include "stdafx.h"
#include "Vat_context.h"
#include "VatCorrectionEngine.h"
#include "SequenceBuilderBase.h"
#include "AsyncSequenceRunner.h"
#include "IVatActuator.h"
#include "IDataRepository.h"
#include "Types.h" 

#include <boost/make_shared.hpp>
#include <boost/move/move.hpp>

namespace DVH_VAT
{
    VatCorrectionEngine::VatCorrectionEngine(SequenceBuilderPtr builder,
                                            VatContextPtr ctx, VatActuatorPtr actuator)
        : m_actuator(actuator)
        , m_pBuilder(builder)
        , m_pCtx(ctx)
    {
        if (!m_pRunner)
        {
            m_pRunner = boost::make_shared<AsyncSequenceRunner>();
        }
    }

    VatCorrectionEngine::~VatCorrectionEngine()
    {
    }

    DataRepositoryPtr VatCorrectionEngine::getRepository() const
    {
        if (m_pCtx)
        {
            return m_pCtx->getRepository();
        }
        return DataRepositoryPtr();
    }

    void VatCorrectionEngine::SetBuilder(SequenceBuilderPtr builder)
    {
        m_pBuilder = builder;
    }

    void VatCorrectionEngine::SetRunner(AsyncSequenceRunnerPtr runner)
    {
        if (runner)
        {
            m_pRunner = runner;
        }
        else
        {
            if (!m_pRunner)
            {
                m_pRunner = boost::make_shared<AsyncSequenceRunner>();
            }
        }
    }

    bool VatCorrectionEngine::RunSequence(std::string sequenceName)
    {
        if (!m_pBuilder)
        {
            return false;
        }

        if (!m_pRunner)
        {
            return false;
        }

        boost::unique_ptr<IVatSequence> seq = m_pBuilder->CreateSequence(sequenceName);

        if (!seq) return false;

        return m_pRunner->Start(
            boost::move(seq),
            m_pCtx,
            m_actuator
        );
    }

    void VatCorrectionEngine::StopSequence()
    {
        if (m_pRunner)
        {
            m_pRunner->Stop();
        }
    }

} // namespace DVH_VAT
