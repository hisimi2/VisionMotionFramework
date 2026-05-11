#pragma once

#include "VatCorrectionEngine.h"
#include "IVatSequenceStrategy.h"
#include "IResultSink.h"

// boost 대신 C++ 표준 라이브러리 사용
#include <mutex>
#include <memory>
#include <vector>
#include <string>

// Windows 헤더 최소화 (UINT 사용목적)
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
// 비 Windows 환경용 더미 타입 (필요에 따라 구성)
typedef unsigned int UINT;
#endif

class CWnd; 

namespace VMF
{
    class IVatAdapter;

    struct VisionResultPayload
    {
        int requestId;
        std::vector<std::string> results;
    };

    // 엔진과 UI 사이 어댑터: UI 의존성은 이 클래스에 국한 (IResultSink 구현)
    class VMF_API CVatEngineUiAdapter : public IResultSink
    {
    public:
        CVatEngineUiAdapter();
        
        ~CVatEngineUiAdapter() override;

        // IResultSink 구현
        void NotifyVisionResult(int requestId, const std::vector<std::string>& results) override;

        // 다이얼로그(또는 윈도우) 소유자 등록
        void SetOwner(::CWnd* pOwner);

        // 메시지 ID를 얻음 (RegisterWindowMessage 기반)
        static UINT GetVisionResultMsgId();

        // 시퀀스 시작/정지 API (외부에서 호출)
        template <typename StrategyType>
        bool StartVatSequence(IVatActuator* adapter)
        {
            // std::lock_guard 사용
            std::lock_guard<std::mutex> guard(m_seqMutex);
            
            // C++11 std::make_shared 사용
            VatSequenceStrategyPtr strategy = std::make_shared<StrategyType>();
			strategy->SetActuator(adapter);
            return StartVatSequenceSafe(strategy);
        }

        void StopVatSequence();

        // 데이터 저장소 접근자
        DataRepositoryPtr getDataRepository();

    protected:
        // 엔진/전략 포인터는 파생 클래스에서 접근 가능하도록 함
        VatSequenceStrategyPtr  m_pCurrentStrategy;
        VatEnginePtr            m_pVatEngine;

        // 파생 클래스가 오버라이드하여 컨텍스트를 맞춤 생성 가능하도록 함
        virtual VatContextPtr CreateContext(const VisionEventHandlerPtr& vm, DataRepositoryPtr& repo);

        // 비전 결과 처리 포인트: 기본 구현은 UI로 전달(PostVisionResult).
        // 파생 클래스는 이 메서드를 재정의하여 자체 처리를 할 수 있음.
        virtual void OnVisionResult(int requestId,  const std::vector<std::string>& results);

        // 구현 세부사항: UI로 결과를 포스트. 외부 공개 필요 없음 -> protected
        void PostVisionResult(int requestId, const std::vector<std::string>& results);

    private:
        // C++11 std::mutex 사용
        mutable std::mutex m_seqMutex;
        bool StartVatSequenceSafe(VatSequenceStrategyPtr strategy);

        ::CWnd* m_pOwner;                // UI 소유자 (옵션)
        static UINT s_msgVisionResult;   // 런타임에 RegisterWindowMessage로 할당
    };
} // namespace VMF
