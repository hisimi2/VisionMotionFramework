#include "stdafx.h"
#include "AsyncSequenceRunner.h"
#include "VAT_Context.h"

#include "IVatSequence.h"
#include "IVatActuator.h"
#include "IResultSink.h"

#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/move/move.hpp>

#include <vector>
#include <string>

namespace DVH_VAT
{
    struct SequenceThreadFunc
    {
        IVatSequence*   m_seq;
        VAT_Context*    m_ctx;
        IVatActuator*   m_act;

        boost::atomic<bool>* m_runningFlag;
        AsyncSequenceRunner* m_runner;

        SequenceThreadFunc(IVatSequence* seq, VAT_Context* ctx, IVatActuator* act, boost::atomic<bool>* runningFlag, AsyncSequenceRunner* runner)
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

    struct AsyncSequenceRunner::Impl
    {
        boost::thread                   thread;
        boost::atomic<bool>             running;
        mutable boost::mutex               mutex;

        boost::unique_ptr<IVatSequence> currentSeq;
		boost::shared_ptr<VAT_Context>  currentCtx;

        IResultSink*                    resultSink;

        Impl()
            : running(false)
            , currentSeq()
            , currentCtx(NULL)
            , resultSink(NULL)
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
            LockGuardType lk(mutex);
            resultSink = sink;
        }

        void sendResultToSink(int requestId, const std::vector<std::string>& results)
        {
            IResultSink* sink = NULL;
            {
                LockGuardType lk(mutex);
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

    AsyncSequenceRunner::AsyncSequenceRunner()
        : m_impl(new Impl())
    {
    }

    AsyncSequenceRunner::~AsyncSequenceRunner()
    {
        Stop();

        if (m_impl)
        {
            if (m_impl->thread.joinable())
            {
                try { m_impl->thread.join(); } catch (...) {}
            }
            m_impl->currentSeq.reset();
            m_impl->currentCtx = NULL;
            m_impl.reset();
        }
    }

    void AsyncSequenceRunner::SetResultSink(IResultSink* sink)
    {
        if (m_impl) m_impl->setResultSink(sink);
    }

    void AsyncSequenceRunner::SendResultToSink(int requestId, const std::vector<std::string>& results)
    {
        if (m_impl) m_impl->sendResultToSink(requestId, results);
    }

    void AsyncSequenceRunner::SendResult(int requestId, const std::string& status)
    {
        if (m_impl) m_impl->sendResult(requestId, status);
    }

    bool AsyncSequenceRunner::Start(boost::unique_ptr<IVatSequence> seq,
		                            boost::shared_ptr<VAT_Context> ctx,
                                    IVatActuator* actuator)
    {
        if (!m_impl) return false;
        if (!seq) return false;

        {
            boost::mutex::scoped_lock lock(m_impl->mutex);

            if (m_impl->running.load()) return false;

            if (m_impl->thread.joinable())
            {
                lock.unlock();
                m_impl->thread.join();
                lock.lock();
            }

            m_impl->running.store(true);
            m_impl->currentSeq = boost::move(seq);
            m_impl->currentCtx = ctx;
        }

        try
        {
            SequenceThreadFunc func(m_impl->currentSeq.get(), ctx.get(), actuator, &m_impl->running, this);
            m_impl->thread = boost::thread(func);
        }
        catch (...)
        {
            boost::mutex::scoped_lock lock(m_impl->mutex);
            m_impl->running.store(false);
            m_impl->currentSeq.reset();
            m_impl->currentCtx = NULL;
            return false;
        }

        return true;
    }

    bool AsyncSequenceRunner::WaitForCompletion(int timeoutMs)
    {
        if (!m_impl) return true;

        const int pollIntervalMs = 10;
        int waited = 0;

        if (timeoutMs < 0)
        {
            while (m_impl->running.load())
            {
                boost::this_thread::sleep_for(boost::chrono::milliseconds(pollIntervalMs));
            }
        }
        else
        {
            while (m_impl->running.load() && waited < timeoutMs)
            {
                boost::this_thread::sleep_for(boost::chrono::milliseconds(pollIntervalMs));
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
            boost::mutex::scoped_lock lock(m_impl->mutex);
            m_impl->currentSeq.reset();
            m_impl->currentCtx = NULL;
            m_impl->running.store(false);
        }

        return true;
    }

    void AsyncSequenceRunner::Abort()
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

    void AsyncSequenceRunner::Stop()
    {
        if (!m_impl) return;
        boost::mutex::scoped_lock lock(m_impl->mutex);
        if (m_impl->running.load() && m_impl->currentCtx)
        {
            m_impl->currentCtx->SetStopRequested(true);
        }
    }

    bool AsyncSequenceRunner::IsRunning() const
    {
        if (!m_impl) return false;
        return m_impl->running.load();
    }

} // namespace DVH_VAT
