#pragma once

#include "VMF_API.h"
#include "Types.h"
#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>

namespace VMF 
{
    class Context;
    class ISequence;
    class IActuator;
    class IResultSink;

    class VMF_API AsyncExecutor 
    {
    public:
        AsyncExecutor();
        
        virtual ~AsyncExecutor();

        bool Start(std::unique_ptr<ISequence> seq, std::shared_ptr<Context> ctx, IActuator* actuator);
        
        void Abort();
        void Stop();
        bool IsRunning() const;
        bool WaitForCompletion(int timeoutMs = -1);

        virtual void SetResultSink(IResultSink* sink);

        void SendResult(int requestId, const std::string& status);

    private:
        std::thread                     m_thread;
        std::atomic<bool>               m_running;
        mutable std::mutex              m_mutex;

        std::unique_ptr<ISequence>      m_currentSeq;
        std::shared_ptr<Context>        m_currentCtx;

        IResultSink*                    m_resultSink;

        void SendResultToSink(int requestId, const std::vector<std::string>& results);
    };
}
