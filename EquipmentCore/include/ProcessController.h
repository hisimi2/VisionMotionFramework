#pragma once
#include "EC_API.h"

#include "Context.h"
#include "ActivityBuilderBase.h"
#include "AsyncExecutor.h"
#include <string>

namespace EC
{
    class ActivityBuilderBase;
    class AsyncExecutor;

    class EC_API ProcessController
    {
    public:
        ProcessController(ActivityBuilderPtr builder, ContextPtr ctx);
        ~ProcessController()= default;

        void SetBuilder(ActivityBuilderPtr builder);
        void SetRunner(AsyncExecutorPtr runner = nullptr); 
        bool RunSequence(const std::string& Name);
        void StopSequence();

    private:
        ActivityBuilderPtr      m_pBuilder;
        AsyncExecutorPtr        m_pRunner;
        ContextPtr              m_pCtx;
    };
} 

