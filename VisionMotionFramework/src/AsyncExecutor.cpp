#include "stdafx.h"
#include "AsyncExecutor.h"
#include "Context.h"

#include "ISequence.h"
#include "IActuator.h"
#include "IResultSink.h"

#include <chrono>
#include <thread>
#include <atomic>
#include <memory>
#include <vector>
#include <string>
#include <mutex>

namespace VMF
{
    struct SequenceThreadFunc
    {
        ISequence*      m_seq;
        Context*        m_ctx;
        IActuator*      m_act;

        std::atomic<bool>* m_runningFlag;
        AsyncExecutor* m_runner;

        SequenceThreadFunc(ISequence* seq, Context* ctx, IActuator* act, std::atomic<bool>* runningFlag, AsyncExecutor* runner)
            : m_seq(seq), m_ctx(ctx), m_act(act), m_runningFlag(runningFlag), m_runner(runner)
        {
        }

        void operator()()
        {
            if (m_runner)
            {
                m_runner->SendResult(0, "started");
            }

            try
            {
                if (m_seq && m_ctx)
                {
                    m_seq->Execute(*m_ctx, m_act);
                }

                if (m_runner)
                    m_runner->SendResult(1, "completed");
            }
            catch (const std::exception& ex)
            {
                if (m_runner)
                {
                    std::string status = std::string("aborted_exception: ") + ex.what();
                    m_runner->SendResult(-1, status);
                }
            }
            catch (...)
            {
                if (m_runner)
                {
                    m_runner->SendResult(-1, "aborted_unknown");
                }
            }

            if (m_runningFlag)
            {
                m_runningFlag->store(false);
            }
        }
    };

    AsyncExecutor::AsyncExecutor()
        : m_running(false)
        , m_currentSeq()
        , m_currentCtx(nullptr)
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

    void AsyncExecutor::SetResultSink(IResultSink* sink)
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_resultSink = sink;
    }

    void AsyncExecutor::SendResultToSink(int requestId, const std::vector<std::string>& results)
    {
        IResultSink* sink = nullptr;
        {
            std::lock_guard<std::mutex> lk(m_mutex);
            sink = m_resultSink;
        }
        if (sink)
        {
            sink->NotifyVisionResult(requestId, results);
        }
    }

    void AsyncExecutor::SendResult(int requestId, const std::string& status)
    {
        if (m_currentSeq)
        {
            std::vector<std::string> msg;
            msg.push_back(std::string("Sequence: ") + m_currentSeq->GetSequenceName());
            msg.push_back(std::string("Task: ") + m_currentSeq->GetTaskName());
            msg.push_back(std::string("Status: ") + status);
            SendResultToSink(requestId, msg);
        }
    }

    bool AsyncExecutor::Start(std::unique_ptr<ISequence> seq,
                                    std::shared_ptr<Context> ctx,
                                    IActuator* actuator)
    {
        if (!seq) return false;

        {
            std::unique_lock<std::mutex> lock(m_mutex);

            if (m_running.load()) return false;

            if (m_thread.joinable())
            {
                lock.unlock();
                m_thread.join();
                lock.lock();
            }

            m_running.store(true);
            m_currentSeq = std::move(seq);
            m_currentCtx = ctx;
        }

        try
        {
            SequenceThreadFunc func(m_currentSeq.get(), ctx.get(), actuator, &m_running, this);
            m_thread = std::thread(func);
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
                std::vector<std::string> msg;
                msg.push_back("Error: Failed to join sequence thread in WaitForCompletion.");
                SendResultToSink(-1, msg);
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
                try
                {
                    m_thread.join();
                }
                catch (...)
                {
                    std::vector<std::string> msg;
                    msg.push_back("Error: Failed to join sequence thread during abort.");
                    SendResultToSink(-1, msg);
                }
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

} // namespace VMF
