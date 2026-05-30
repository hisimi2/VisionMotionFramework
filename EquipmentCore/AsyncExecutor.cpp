#include "stdafx.h"
#include "AsyncExecutor.h"
#include "Context.h"

#include "ISequence.h"

#include <chrono>
#include <thread>
#include <atomic>
#include <memory>
#include <vector>
#include <string>
#include <mutex>

namespace EC
{
    struct SequenceThreadFunc
    {
        ISequence*      m_seq;
        Context*        m_ctx;
        std::atomic<bool>* m_runningFlag;
        AsyncExecutor*  m_runner;

        SequenceThreadFunc(ISequence* seq, Context* ctx, std::atomic<bool>* runningFlag, AsyncExecutor* runner)
            : m_seq(seq), m_ctx(ctx), m_runningFlag(runningFlag), m_runner(runner)
        {
        }

        void operator()()
        {
            try
            {
                if (m_seq && m_ctx)
                {
                    m_seq->Execute(*m_ctx);
                }
            }
            catch (const std::exception& ex)
            {
                // 예외 발생 시 로깅 (SendResult는 별도 sink 연결 필요)
            }
            catch (...)
            {
            }

            if (m_runningFlag)
            {
                m_runningFlag->store(false);
            }
        }
    };

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

    bool AsyncExecutor::Start(std::unique_ptr<ISequence> seq,
                                    std::shared_ptr<Context> ctx)
    {
        if (!seq) return false;

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
        }

        try
        {
            SequenceThreadFunc func(m_currentSeq.get(), ctx.get(), &m_running, this);
            m_thread = std::thread(std::move(func));
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
            m_currentCtx->SetStopRequested(true);
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
            m_resultSink->NotifyVisionResult(requestId, results);
        }
    }

} // namespace EC
