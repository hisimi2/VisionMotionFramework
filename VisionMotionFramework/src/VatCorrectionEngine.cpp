#include "stdafx.h"
#include "Vat_context.h"
#include "VatCorrectionEngine.h"
#include "SequenceBuilderBase.h"
#include "AsyncSequenceRunner.h"
#include "IVatActuator.h"
#include "IDataRepository.h"
#include "Types.h" 

#include <memory> // C++11 std::make_shared 등 대체 사용
#include <string>
#include <utility> // std::move

namespace VMF
{
    VatCorrectionEngine::VatCorrectionEngine(SequenceBuilderPtr builder,
                                            VatContextPtr ctx, VatActuatorPtr actuator)
        : m_actuator(actuator)
        , m_pBuilder(builder)
        , m_pCtx(ctx)
    {
        if (!m_pRunner)
        {
            m_pRunner = std::make_shared<AsyncSequenceRunner>(); // boost::make_shared 대체
        }
    }

    // 소멸자 (헤더에서 선언된 명시적 소멸자와 매칭. 빈 동작이므로 default 선언 권장이나 이 파일에 유지)
    VatCorrectionEngine::~VatCorrectionEngine() = default;

    DataRepositoryPtr VatCorrectionEngine::getRepository() const
    {
        if (m_pCtx)
        {
            return m_pCtx->getRepository();
        }
        return nullptr; // C++11 nullptr 사용
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
                m_pRunner = std::make_shared<AsyncSequenceRunner>();
            }
        }
    }

    // C++14: 헤더 수정 사항에 맞춰 const std::string& 형태로 매개변수 일치
    bool VatCorrectionEngine::RunSequence(const std::string& sequenceName)
    {
        if (!m_pBuilder)
        {
            return false;
        }

        if (!m_pRunner)
        {
            return false;
        }

        // boost::unique_ptr 가 아닌 std::unique_ptr 반환
        std::unique_ptr<IVatSequence> seq = m_pBuilder->CreateSequence(sequenceName);

        if (!seq) return false;

        return m_pRunner->Start(
            std::move(seq), // boost::move -> std::move 적용
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

} // namespace VMF
