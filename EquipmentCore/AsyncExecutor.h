#pragma once

#include "EC_API.h"
#include <memory>
#include <vector>
#include <string>

namespace EC 
{
    class Context;
    class ISequence;
    class IActuator;
    class IResultSink;

    class EC_API AsyncExecutor 
    {
    public:
        AsyncExecutor();
        
        // 가상 함수가 존재하므로 안전한 상속을 위해 가상 소멸자로 변경하는 것을 권장합니다.
        virtual ~AsyncExecutor();

        bool Start(std::unique_ptr<ISequence> seq, std::shared_ptr<Context> ctx, IActuator* actuator);
        
        void Abort();
        void Stop();
        bool IsRunning() const;
        bool WaitForCompletion(int timeoutMs = -1);

        virtual void SetResultSink(IResultSink* sink);

        void SendResult(int requestId, const std::string& status);

    private:
        struct Impl;
        
        std::unique_ptr<Impl> m_impl;

        void SendResultToSink(int requestId, const std::vector<std::string>& results);
    };
}
