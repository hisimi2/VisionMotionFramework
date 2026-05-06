#pragma once

#include "DVH_VAT_API.h"
#include "Types.h"
#include <boost/move/unique_ptr.hpp>
#include <vector>
#include <string>

namespace DVH_VAT 
{
    class VAT_Context;
    class IVatSequence;
    class IVatActuator;
    class IResultSink;

    class DVH_VAT_API AsyncSequenceRunner 
    {
    public:
        AsyncSequenceRunner();
        ~AsyncSequenceRunner();

        bool Start(boost::unique_ptr<IVatSequence> seq, boost::shared_ptr<VAT_Context> ctx, IVatActuator* actuator);
        void Abort();
        void Stop();
        bool IsRunning() const;
        bool WaitForCompletion(int timeoutMs = -1);

        virtual void SetResultSink(IResultSink* sink);

        void SendResult(int requestId, const std::string& status);

    private:
        struct Impl;
        boost::scoped_ptr<Impl> m_impl;

        void SendResultToSink(int requestId, const std::vector<std::string>& results);
    };
}
