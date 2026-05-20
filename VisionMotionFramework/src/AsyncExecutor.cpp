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
        ISequence*   m_seq;
        Context*    m_ctx;
        IActuator*   m_act;

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

    struct AsyncExecutor::Impl
    {
        std::thread                     thread;
        std::atomic<bool>               running;
        mutable std::mutex              mutex;

        std::unique_ptr<ISequence>   currentSeq;
        std::shared_ptr<Context>    currentCtx;

        IResultSink*                    resultSink;

        Impl()
            : running(false)
            , currentSeq()
            , currentCtx(nullptr)
            , resultSink(nullptr)
        {
        }

        ~Impl()
        {
            if (thread.joinable())
            {
                try { thread.join(); } catch (...) {}
            }
            currentSeq.reset();
        }

        void setResultSink(IResultSink* sink)
        {
            std::lock_guard<std::mutex> lk(mutex); 
            resultSink = sink;
        }

        void sendResultToSink(int requestId, const std::vector<std::string>& results)
        {
            IResultSink* sink = nullptr;
            {
                std::lock_guard<std::mutex> lk(mutex);
                sink = resultSink;
            }
            if (sink)
            {
                sink->NotifyVisionResult(requestId, results);
            }
        }

        void sendResult(int requestId, const std::string& status)
        {
            if (currentSeq)
            {
                std::vector<std::string> msg;
                msg.push_back(std::string("Sequence: ") + currentSeq->GetSequenceName());
                msg.push_back(std::string("Task: ") + currentSeq->GetTaskName());
                msg.push_back(std::string("Status: ") + status);
                sendResultToSink(requestId, msg);
            }
        }
    };

    AsyncExecutor::AsyncExecutor()
        : m_impl(std::make_unique<Impl>()) // C++14: std::make_unique 적용
    {
    }

    AsyncExecutor::~AsyncExecutor()
    {
        Stop();

        if (m_impl)
        {
            if (m_impl->thread.joinable())
            {
                try { m_impl->thread.join(); } catch (...) {}
            }
            m_impl->currentSeq.reset();
            m_impl->currentCtx = nullptr;
            m_impl.reset();
        }
    }

    void AsyncExecutor::SetResultSink(IResultSink* sink)
    {
        if (m_impl) m_impl->setResultSink(sink);
    }

    void AsyncExecutor::SendResultToSink(int requestId, const std::vector<std::string>& results)
    {
        if (m_impl) m_impl->sendResultToSink(requestId, results);
    }

    void AsyncExecutor::SendResult(int requestId, const std::string& status)
    {
        if (m_impl) m_impl->sendResult(requestId, status);
    }

    bool AsyncExecutor::Start(std::unique_ptr<ISequence> seq,
                                    std::shared_ptr<Context> ctx,
                                    IActuator* actuator)
    {
        if (!m_impl) return false;
        if (!seq) return false;

        {
            std::unique_lock<std::mutex> lock(m_impl->mutex); // scoped_lock -> unique_lock

            if (m_impl->running.load()) return false;

            if (m_impl->thread.joinable())
            {
                lock.unlock();
                m_impl->thread.join();
                lock.lock();
            }

            m_impl->running.store(true);
            m_impl->currentSeq = std::move(seq); 
            m_impl->currentCtx = ctx;
        }

        try
        {
            SequenceThreadFunc func(m_impl->currentSeq.get(), ctx.get(), actuator, &m_impl->running, this);
            m_impl->thread = std::thread(func); 
        }
        catch (...)
        {
            std::lock_guard<std::mutex> lock(m_impl->mutex);
            m_impl->running.store(false);
            m_impl->currentSeq.reset();
            m_impl->currentCtx = nullptr;
            return false;
        }

        return true;
    }

    bool AsyncExecutor::WaitForCompletion(int timeoutMs)
    {
        if (!m_impl) return true;

        const int pollIntervalMs = 10;
        int waited = 0;

        if (timeoutMs < 0)
        {
            while (m_impl->running.load())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(pollIntervalMs)); 
            }
        }
        else
        {
            while (m_impl->running.load() && waited < timeoutMs)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(pollIntervalMs));
                waited += pollIntervalMs;
            }
        }

        if (m_impl->thread.joinable())
        {
            try
            {
                m_impl->thread.join();
            }
            catch (...)
            {
                std::vector<std::string> msg;
                msg.push_back("Error: Failed to join sequence thread in WaitForCompletion.");
                m_impl->sendResultToSink(-1, msg);
                return false;
            }
        }

        {
            std::lock_guard<std::mutex> lock(m_impl->mutex);
            m_impl->currentSeq.reset();
            m_impl->currentCtx = nullptr;
            m_impl->running.store(false);
        }

        return true;
    }

    void AsyncExecutor::Abort()
    {
        Stop();

        if (!WaitForCompletion(5000))
        {
            if (m_impl && m_impl->thread.joinable())
            {
                try
                {
                    m_impl->thread.join();
                }
                catch (...)
                {
                    std::vector<std::string> msg;
                    msg.push_back("Error: Failed to join sequence thread during abort.");
                    if (m_impl) m_impl->sendResultToSink(-1, msg);
                }
            }
        }
    }

    void AsyncExecutor::Stop()
    {
        if (!m_impl) return;
        std::lock_guard<std::mutex> lock(m_impl->mutex);
        if (m_impl->running.load() && m_impl->currentCtx)
        {
            m_impl->currentCtx->SetStopRequested(true);
        }
    }

    bool AsyncExecutor::IsRunning() const
    {
        if (!m_impl) return false;
        return m_impl->running.load();
    }

} // namespace VMF
