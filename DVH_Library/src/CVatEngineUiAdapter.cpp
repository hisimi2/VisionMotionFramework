#include "stdafx.h"
#include "CVatEngineUiAdapter.h"
#include "VatCorrectionEngine.h"
#include "VAT_Context.h" // VatContextPtr 정의
#include "CompatUtils.h"

#include "AsyncSequenceRunner.h" // 추가: runner 생성 및 SetResultSink 호출을 위해 포함

#include <sstream>
#include <afxwin.h> // CWnd, RegisterWindowMessage
#include <tchar.h>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace DVH_VAT
{
    UINT CVatEngineUiAdapter::s_msgVisionResult = 0;

    CVatEngineUiAdapter::CVatEngineUiAdapter()
        : m_pOwner(nullptr)
        , m_pVatEngine()
    {
    }

    CVatEngineUiAdapter::~CVatEngineUiAdapter()
    {
        VatEnginePtr engineToStop;
        VatSequenceStrategyPtr strategyToReset;
        {
            LockGuardType guard(m_seqMutex);
            engineToStop = m_pVatEngine;
            m_pVatEngine.reset();
            strategyToReset = m_pCurrentStrategy;
            m_pCurrentStrategy.reset();
        }

        if (engineToStop)
        {
            engineToStop->StopSequence();
            engineToStop.reset();
        }
    }

    // 헤더에 선언된 이름(getDataRepository)과 일치하도록 구현
    DataRepositoryPtr CVatEngineUiAdapter::getDataRepository()
    {
        LockGuardType guard(m_seqMutex);
        if (m_pVatEngine)
        {
            return m_pVatEngine->getRepository();
        }
        return DataRepositoryPtr();
    }

    void CVatEngineUiAdapter::SetOwner(::CWnd* pOwner)
    {
        LockGuardType guard(m_seqMutex);
        m_pOwner = pOwner;
    }

    UINT CVatEngineUiAdapter::GetVisionResultMsgId()
    {
        if (s_msgVisionResult == 0)
        {
            s_msgVisionResult = ::RegisterWindowMessage(_T("DVH_VAT_VISION_RESULT_v100"));
            if (s_msgVisionResult == 0)
            {
                s_msgVisionResult = WM_USER + 0x0400;
            }
        }
        return s_msgVisionResult;
    }

    // IResultSink 구현: 외부 엔티티에서 호출되는 진입점
    void CVatEngineUiAdapter::NotifyVisionResult(int requestId, const std::vector<std::string>& results)
    {
        // 내부 처리 (UI로 포스트)
        OnVisionResult(requestId, results);
    }

    void CVatEngineUiAdapter::PostVisionResult(int requestId, const std::vector<std::string>& results)
    {
        LockGuardType guard(m_seqMutex);

        if (!m_pOwner || !::IsWindow(m_pOwner->GetSafeHwnd()))
            return;

        // smart pointer로 소유권을 묶어 메시지로 전달: 수신측에서 shared_ptr을 복원하여 사용
        boost::shared_ptr<VisionResultPayload> sp = boost::make_shared<VisionResultPayload>();
        sp->requestId = requestId;
        sp->results = results;

        // 힙에 shared_ptr을 복사하여 전달
        boost::shared_ptr<VisionResultPayload>* heap_sp = new boost::shared_ptr<VisionResultPayload>(sp);

        ::PostMessage(m_pOwner->GetSafeHwnd(), GetVisionResultMsgId(), reinterpret_cast<WPARAM>(heap_sp), 0);
    }

    VatContextPtr CVatEngineUiAdapter::CreateContext(const VisionEventHandlerPtr& vm, DataRepositoryPtr& repo)
    {
        auto ctx = boost::make_shared<VAT_Context>();
        ctx->SetVisionProcessor(vm);
        ctx->SetDataRepository(repo);
        return ctx;
    }

    void CVatEngineUiAdapter::OnVisionResult(int requestId, const std::vector<std::string>& results)
    {
        PostVisionResult(requestId, results);
    }

    bool CVatEngineUiAdapter::StartVatSequenceSafe(VatSequenceStrategyPtr strategy)
    {
        if (!strategy)
        {
            return false;
        }

        m_pCurrentStrategy = strategy;

        if (m_pVatEngine)
        {
            m_pVatEngine->StopSequence();
            m_pVatEngine.reset();
        }

        SequenceBuilderPtr      builder;
        DataRepositoryPtr       repo;
        VisionEventHandlerPtr   vm;

        try
        {
            builder = strategy->CreateBuilder();
            repo = strategy->CreateRepository();
            vm = strategy->CreateVisionProcessor();
        }
        catch (...)
        {
            m_pCurrentStrategy.reset();
            return false;
        }

        if (!builder || !vm || !repo)
        {
            m_pCurrentStrategy.reset();
            return false;
        }

        VatContextPtr ctx;
        try
        {
            ctx = CreateContext(vm, repo);
        }
        catch (...)
        {
            m_pCurrentStrategy.reset();
            return false;
        }

        if (!ctx)
        {
            m_pCurrentStrategy.reset();
            return false;
        }

        try
        {
            strategy->ConfigureParams(ctx);
        }
        catch (const std::exception& ex)
        {
            ctx->SetLastError(ex.what());
            m_pCurrentStrategy.reset();
            return false;
        }
        catch (...)
        {
            ctx->SetLastError("Unknown exception in ConfigureParams");
            m_pCurrentStrategy.reset();
            return false;
        }

        VatActuatorPtr actuator = strategy->GetActuator();

        try
        {
            m_pVatEngine = boost::make_shared<VatCorrectionEngine>(builder, ctx, actuator);
        }
        catch (const std::exception& ex)
        {
            ctx->SetLastError(ex.what());
            m_pVatEngine.reset();
            m_pCurrentStrategy.reset();
            return false;
        }
        catch (...)
        {
            ctx->SetLastError("Unknown exception creating VatCorrectionEngine");
            m_pVatEngine.reset();
            m_pCurrentStrategy.reset();
            return false;
        }

        // 추가: runner를 새로 만들고 이 어댑터를 결과 sink로 등록한 다음 엔진에 주입
        {
            AsyncSequenceRunnerPtr runner = boost::make_shared<AsyncSequenceRunner>();
            runner->SetResultSink(this); // 등록: runner가 결과를 어댑터로 보냄
            m_pVatEngine->SetRunner(runner);
        }

        std::string seqName = strategy->GetSequenceName();
        if (!m_pVatEngine->RunSequence(seqName))
        {
            m_pVatEngine->StopSequence();
            m_pVatEngine.reset();
            m_pCurrentStrategy.reset();
            return false;
        }

        return true;
    }

	void CVatEngineUiAdapter::StopVatSequence()
	{
		VatEnginePtr engineToStop;
		{
			boost::lock_guard<boost::mutex> guard(m_seqMutex);
			engineToStop = m_pVatEngine;
			m_pVatEngine.reset();
			m_pCurrentStrategy.reset();
		}

		if (engineToStop)
		{
			engineToStop->StopSequence();
		}
	}
} // namespace DVH_VAT
