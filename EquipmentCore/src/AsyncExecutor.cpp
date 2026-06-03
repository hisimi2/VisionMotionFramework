#include "stdafx.h"
#include "AsyncExecutor.h"
#include "Context.h"

#include "IActivity.h"

#include <chrono>
#include <thread>
#include <atomic>
#include <memory>
#include <vector>
#include <string>
#include <mutex>

namespace EC
{
    AsyncExecutor::AsyncExecutor()
        : m_running(false)
        , m_resultSink(nullptr)
    {
    }

    AsyncExecutor::~AsyncExecutor()
    {
        Stop();

        if (m_thread.joinable())
        {
            try { m_thread.join(); } catch (...) {}
        }
        m_currentSeq.reset();
        m_currentCtx = nullptr;
    }

    bool AsyncExecutor::Start(std::unique_ptr<IActivity> seq, std::shared_ptr<Context> ctx)
    {
        if (!seq) return false;

        IActivity* rawSeq = nullptr;
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if (m_running.load()) return false;

            if (m_thread.joinable())
            {
                m_thread.join();
            }

            m_running.store(true);
            m_currentSeq = std::move(seq);
            m_currentCtx = ctx;

            // Context에 Executor 설정 → Task가 결과를 전송할 수 있도록 함
            ctx->SetExecutor(this);

            rawSeq = m_currentSeq.get();
        }

        try
        {
            m_thread = std::thread(
                [this, rawSeq, ctx]()
                {
                    try
                    {
                        if (rawSeq && ctx)
                        {
                            rawSeq->Execute(*ctx);
                        }
                    }
                    catch (...)
                    {
                    }

                    m_running.store(false);
                });
        }
        catch (...)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_running.store(false);
            m_currentSeq.reset();
            m_currentCtx = nullptr;
            return false;
        }

        return true;
    }

    bool AsyncExecutor::WaitForCompletion(int timeoutMs)
    {
        const int pollIntervalMs = 10;
        int waited = 0;

        if (timeoutMs < 0)
        {
            while (m_running.load())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(pollIntervalMs));
            }
        }
        else
        {
            while (m_running.load() && waited < timeoutMs)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(pollIntervalMs));
                waited += pollIntervalMs;
            }
        }

        if (m_thread.joinable())
        {
            try
            {
                m_thread.join();
            }
            catch (...)
            {
                return false;
            }
        }

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_currentSeq.reset();
            m_currentCtx = nullptr;
            m_running.store(false);
        }

        return true;
    }

    void AsyncExecutor::Abort()
    {
        Stop();

        if (!WaitForCompletion(5000))
        {
            if (m_thread.joinable())
            {
                try { m_thread.join(); } catch (...) {}
            }
        }
    }

    void AsyncExecutor::Stop()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_running.load() && m_currentCtx)
        {
            m_currentCtx->SetStop();
        }
    }

    bool AsyncExecutor::IsRunning() const
    {
        return m_running.load();
    }

    void AsyncExecutor::SetResultSink(IResultSink* sink)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_resultSink = sink;
    }

    void AsyncExecutor::SendResult(int requestId, const std::string& status)
    {
        std::vector<std::string> results;
        results.push_back(status);
        SendResultToSink(requestId, results);
    }

    void AsyncExecutor::SendResultToSink(int requestId, const std::vector<std::string>& results)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_resultSink)
        {
            m_resultSink->NotifyResult(requestId, results);
        }
    }
} 
