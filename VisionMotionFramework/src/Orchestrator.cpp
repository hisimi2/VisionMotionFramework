#include "stdafx.h"
#include "Orchestrator.h"
#include "RunController.h"
#include "Context.h"

#include "AsyncExecutor.h"

#include <memory>
#include <mutex>
#include <exception>

namespace VMF
{
    Orchestrator::Orchestrator()
        : m_pVisionEngine()
    {
    }

    Orchestrator::~Orchestrator()
    {
        VisionEnginePtr engineToStop;
        {
            std::lock_guard<std::mutex> guard(m_seqMutex);
            engineToStop = m_pVisionEngine;
            m_pVisionEngine.reset();
            m_pCurrentStrategy.reset();
        }

         if (engineToStop)
         {
             engineToStop->StopSequence();
             engineToStop.reset();
         }

         ClearObservers();
    }

DataRepositoryPtr Orchestrator::GetDataRepository()
    {
        std::lock_guard<std::mutex> guard(m_seqMutex);
        if (m_pVisionEngine)
        {
            return m_pVisionEngine->GetRepository();
        }
        return m_directDataRepository; // 직접 모드 지원
    }

    Orchestrator::ObserverId Orchestrator::AddObserver(VisionResultObserver observer)
    {
        if (!observer)
            return 0;

        const ObserverId id = m_nextObserverId.fetch_add(1);
        {
            std::lock_guard<std::mutex> lk(m_observerMutex);
            m_observers[id] = std::move(observer);
        }
        return id;
    }

    bool Orchestrator::RemoveObserver(ObserverId id)
    {
        std::lock_guard<std::mutex> lk(m_observerMutex);
        return m_observers.erase(id) >0;
    }

    void Orchestrator::ClearObservers()
    {
        std::lock_guard<std::mutex> lk(m_observerMutex);
        m_observers.clear();
    }

    void Orchestrator::NotifyVisionResult(int requestId, const std::vector<std::string>& results)
    {
        OnVisionResult(requestId, results);
    }

    void Orchestrator::NotifyObservers(const VisionResultPayload& payload)
    {
        std::vector<VisionResultObserver> snapshot;
        {
            std::lock_guard<std::mutex> lk(m_observerMutex);
            snapshot.reserve(m_observers.size());
            for (auto& kv : m_observers)
            {
                if (kv.second)
                snapshot.push_back(kv.second);
            }
        }

        for (auto& cb : snapshot)
        {
            try
            {
            cb(payload);
            }
            catch (...)
            {
            // ignore observer exceptions
            }
        }
    }

    VisionContextPtr Orchestrator::CreateContext(const VisionProcessorPtr& vm, DataRepositoryPtr& repo)
    {
        auto ctx = std::make_shared<Context>();
        ctx->SetVisionProcessor(vm);
        ctx->SetDataRepository(repo);
        return ctx;
    }

    void Orchestrator::OnVisionResult(int requestId, const std::vector<std::string>& results)
    {
        VisionResultPayload payload;
        payload.requestId = requestId;
        payload.results = results;
        NotifyObservers(payload);
    }

    void Orchestrator::StopSequence()
    {
        VisionEnginePtr engineToStop;
        {
            std::lock_guard<std::mutex> guard(m_seqMutex);
            engineToStop = m_pVisionEngine;
            m_pVisionEngine.reset();
            m_pCurrentStrategy.reset();
        }

if (engineToStop)
        {
         engineToStop->StopSequence();
        }
    }

    // --- [직접 모드] 구현 ---

    void Orchestrator::SetVisionProcessor(VisionProcessorPtr vp)
    {
        std::lock_guard<std::mutex> guard(m_seqMutex);
        m_directVisionProcessor = vp;

        // 직접 모드: 이미 Context가 생성된 경우에도 VP 연결 업데이트
        if (m_directContext)
        {
            m_directContext->SetVisionProcessor(vp);
        }
    }

    VisionProcessorPtr Orchestrator::GetVisionProcessor() const
    {
        std::lock_guard<std::mutex> guard(m_seqMutex);

        // 상태머신 모드: RunController → Context → VisionProcessor
        if (m_pVisionEngine)
        {
            auto ctx = m_pVisionEngine->GetContext();
            if (ctx) return ctx->GetVisionProcessorInterface();
        }
        return m_directVisionProcessor;
    }

    void Orchestrator::SetDataRepository(DataRepositoryPtr repo)
    {
        std::lock_guard<std::mutex> guard(m_seqMutex);
        m_directDataRepository = repo;

        // 직접 모드: 이미 Context가 생성된 경우에도 Repo 연결 업데이트
        if (m_directContext)
        {
            m_directContext->SetDataRepository(repo);
        }
    }

    VisionContextPtr Orchestrator::GetOrCreateContext()
    {
        std::lock_guard<std::mutex> guard(m_seqMutex);

        // 상태머신 모드: RunController의 Context 반환
        if (m_pVisionEngine)
        {
            auto ctx = m_pVisionEngine->GetContext();
            if (ctx) return ctx;
        }

        // 직접 모드: 없으면 생성 (MemorySequenceStrategy와 동일 패턴)
        if (!m_directContext)
        {
            m_directContext = std::make_shared<Context>();
            if (m_directVisionProcessor)
                m_directContext->SetVisionProcessor(m_directVisionProcessor);
            if (m_directDataRepository)
                m_directContext->SetDataRepository(m_directDataRepository);
        }
        return m_directContext;
    }

    bool Orchestrator::ExecuteDirectVisionCommand(VisionCommand cmd)
    {
        auto ctx = GetOrCreateContext();
        if (!ctx) return false;
        return ctx->ExecuteVisionCommand(cmd);
    }

    bool Orchestrator::ExecuteDirectVisionCommand(VisionCommand cmd, const StringMap& params)
    {
        auto ctx = GetOrCreateContext();
        if (!ctx) return false;
        for (const auto& kv : params)
            ctx->SetVisionParamAs<std::string>(kv.first, kv.second);
        return ctx->ExecuteVisionCommand(cmd);
    }

} // namespace VMF
