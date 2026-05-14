#include "stdafx.h"
#include "Context.h"
#include "SequenceExecutionEngine.h"
#include "SequenceBuilderBase.h"
#include "SequenceExecutionWorker.h"
#include "IActuator.h"
#include "IDataRepository.h"
#include "Types.h" 

#include <memory> // C++11 std::make_shared 등 대체 사용
#include <string>
#include <utility> // std::move

namespace VMF
{
    SequenceExecutionEngine::SequenceExecutionEngine(SequenceBuilderPtr builder,
                                            VatContextPtr ctx, VatActuatorPtr actuator)
        : m_actuator(actuator)
        , m_pBuilder(builder)
        , m_pCtx(ctx)
    {
        if (!m_pRunner)
        {
            m_pRunner = std::make_shared<SequenceExecutionWorker>(); // boost::make_shared 대체
        }
    }

    // 소멸자 (헤더에서 선언된 명시적 소멸자와 매칭. 빈 동작이므로 default 선언 권장이나 이 파일에 유지)
    SequenceExecutionEngine::~SequenceExecutionEngine() = default;

    DataRepositoryPtr SequenceExecutionEngine::getRepository() const
    {
        if (m_pCtx)
        {
            return m_pCtx->getRepository();
        }
        return nullptr; // C++11 nullptr 사용
    }

    void SequenceExecutionEngine::SetBuilder(SequenceBuilderPtr builder)
    {
        m_pBuilder = builder;
    }

    void SequenceExecutionEngine::SetRunner(SequenceExecutionWorkerPtr runner)
    {
        if (runner)
        {
            m_pRunner = runner;
        }
        else
        {
            if (!m_pRunner)
            {
                m_pRunner = std::make_shared<SequenceExecutionWorker>();
            }
        }
    }

    // C++14: 헤더 수정 사항에 맞춰 const std::string& 형태로 매개변수 일치
    bool SequenceExecutionEngine::RunSequence(const std::string& sequenceName)
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
        std::unique_ptr<ISequence> seq = m_pBuilder->CreateSequence(sequenceName);

        if (!seq) return false;

        return m_pRunner->Start(
            std::move(seq), // boost::move -> std::move 적용
            m_pCtx,
            m_actuator
        );
    }

    void SequenceExecutionEngine::StopSequence()
    {
        if (m_pRunner)
        {
            m_pRunner->Stop();
        }
    }

} // namespace VMF
