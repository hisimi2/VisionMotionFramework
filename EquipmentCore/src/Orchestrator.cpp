#include "stdafx.h"
#include "Orchestrator.h"
#include "ProcessController.h"
#include "Context.h"
#include "AsyncExecutor.h"

#include <memory>
#include <mutex>
#include <exception>

namespace EC
{
    Orchestrator::Orchestrator()
        : m_nextObserverId(1)
        , m_pProcess(nullptr)
    {
    }

    Orchestrator::~Orchestrator()
    {
        ProcessControllerPtr engineToStop;
        {
            std::lock_guard<std::mutex> guard(m_seqMutex);
            engineToStop = m_pProcess;
            m_pProcess.reset();
            m_pBuilder.reset();
        }

        if (engineToStop)
        {
            engineToStop->StopActivity();
            engineToStop.reset();
        }

        ClearObservers();
    }

    ContextPtr Orchestrator::CreateContext()
    {
        auto ctx = std::make_shared<Context>();
        return ctx;
    }

    Orchestrator::ObserverId Orchestrator::AddObserver(ResultObserver observer)
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

    void Orchestrator::NotifyResult(int requestId, const std::vector<std::string>& results)
    {
        OnResult(requestId, results);
    }

    void Orchestrator::NotifyObservers(const ResultPayload& payload)
    {
        std::vector<ResultObserver> snapshot;
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

    void Orchestrator::OnResult(int requestId, const std::vector<std::string>& results)
    {
        ResultPayload payload;
        payload.requestId = requestId;
        payload.results = results;
        NotifyObservers(payload);
    }

    bool Orchestrator::StartSequenceSafe(ActivityBuilderPtr builder)
    {
        if (!builder)
        {
            return false;
        }

        m_pBuilder = builder;

        if (m_pProcess)
        {
            m_pProcess->StopActivity();
            m_pProcess.reset();
        }

        ActivityBuilderPtr builder;

        try
        {
            builder->Create();
        }
        catch (...)
        {
            m_pBuilder.reset();
            return false;
        }

        if (!builder )
        {
            m_pBuilder.reset();
            return false;
        }

        ContextPtr ctx;
        try
        {
            ctx = CreateContext();
        }
        catch (...)
        {
            m_pBuilder.reset();
            return false;
        }

        if (!ctx)
        {
            m_pBuilder.reset();
            return false;
        }

        try
        {
            strategy->ConfigureParams(ctx);
        }
        catch (const std::exception& ex)
        {
            ctx->SetLastError(ex.what());
            m_pBuilder.reset();
            return false;
        }
        catch (...)
        {
            ctx->SetLastError("Unknown exception in ConfigureParams");
            m_pBuilder.reset();
            return false;
        }

        try
        {
            m_pProcess = std::make_shared<ProcessController>(builder, ctx);
        }
        catch (const std::exception& ex)
        {
            ctx->SetLastError(ex.what());
            m_pProcess.reset();
            m_pBuilder.reset();
            return false;
        }
        catch (...)
        {
            ctx->SetLastError("Unknown exception creating RunController");
            m_pProcess.reset();
            m_pBuilder.reset();
            return false;
        }

        {
            AsyncExecutorPtr runner = std::make_shared<AsyncExecutor>();
            runner->SetResultSink(this);
            m_pProcess->SetRunner(runner);
        }

        if (!m_pProcess->RunActivity())
        {
            m_pProcess->StopActivity();
            m_pProcess.reset();
            m_pBuilder.reset();
            return false;
        }

        return true;
    }

    void Orchestrator::StopSequence()
    {
        ProcessControllerPtr engineToStop;
        {
            std::lock_guard<std::mutex> guard(m_seqMutex);
            engineToStop = m_pProcess;
            m_pProcess.reset();
            m_pBuilder.reset();
        }

        if (engineToStop)
        {
            engineToStop->StopActivity();
        }
    }
} 
