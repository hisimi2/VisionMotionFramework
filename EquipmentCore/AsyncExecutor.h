#pragma once

#include "EC_API.h"
#include "IResultSink.h"
#include "Context.h"
#include "ISequence.h"

#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <string>

namespace EC 
{
    class EC_API AsyncExecutor 
    {
    public:
        AsyncExecutor();
        virtual ~AsyncExecutor();

        bool Start(std::unique_ptr<ISequence> seq, std::shared_ptr<Context> ctx);
        
        void Abort();
        void Stop();
        bool IsRunning() const;
        bool WaitForCompletion(int timeoutMs = -1);

        virtual void SetResultSink(IResultSink* sink);
        void SendResult(int requestId, const std::string& status);

    private:
        void SendResultToSink(int requestId, const std::vector<std::string>& results);

        std::thread                     m_thread;
        std::atomic<bool>               m_running;
        mutable std::mutex              m_mutex;
        std::unique_ptr<ISequence>      m_currentSeq;
        std::shared_ptr<Context>        m_currentCtx;
        IResultSink*                    m_resultSink;
    };
}
