#include "stdafx.h"
#include "Context.h"
#include "ProcessController.h"
#include "AsyncExecutor.h"

#include <memory> 
#include <string>
#include <utility> 

namespace EC
{
    ProcessController::ProcessController(ActivityBuilderPtr builder, ContextPtr ctx)
        : m_pBuilder(builder)
        , m_pCtx(ctx)
    {
        if (!m_pRunner)
        {
            m_pRunner = std::make_shared<AsyncExecutor>(); 
        }
    }

    void ProcessController::SetBuilder(ActivityBuilderPtr builder)
    {
        m_pBuilder = builder;
    }

    void ProcessController::SetRunner(AsyncExecutorPtr runner)
    {
        if (runner)
        {
            m_pRunner = runner;
        }
        else
        {
            if (!m_pRunner)
            {
                m_pRunner = std::make_shared<AsyncExecutor>();
            }
        }
    }


    bool ProcessController::RunActivity()
    {
        if (!m_pBuilder)
        {
            return false;
        }

        if (!m_pRunner)
        {
            return false;
        }

        std::unique_ptr<IActivity> seq = m_pBuilder->Create();

        if (!seq) return false;

        return m_pRunner->Start
        (
            std::move(seq), 
            m_pCtx
        );
    }

    void ProcessController::StopActivity()
    {
        if (m_pRunner)
        {
            m_pRunner->Stop();
        }
    }
} 
