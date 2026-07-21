#pragma once

#include "RunController.h"
#include "DefaultSetupStrategy.h"
#include "ISequenceSetup.h"
#include "IComponentSetup.h"
#include "IResultSink.h"
#include "IVisionClient.h"
#include "IDataRepository.h"
#include "AsyncExecutor.h"

#include <mutex>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <atomic>
#include <cstdint>

namespace VMF
{
	struct VisionResultPayload
	{
		int requestId;
		std::vector<std::string> results;
	};

	// VAT Sequence Orchestrator (UI 비종속).
	// - (ComponentFactory + SequenceFactory) 주입 → 객체 조립
	// - 상태머신(AsyncExecutor) 또는 직접 모드 실행 지원
	// - Runner 결과를 Observer에게 브로드캐스트
	//
	// ※ 리팩토링 사항:
	//   - 템플릿 메서드(StartSequence<StrategyType>, InitializeDirectWithStrategy<StrategyType> 등) 제거
	//   - 대신 생성자/IoC를 통해 IComponentSetup + ISequenceSetup 팩토리 주입
	//   - StartSequenceFromStrategy는 DefaultSetupStrategy(두 인터페이스 통합) 인자로 변경
	class VMF_API Orchestrator : public IResultSink
	{
	public:
		using ObserverId = std::uint64_t;
		using VisionResultObserver = std::function<void(const VisionResultPayload& payload)>;
        
/// <summary>
		/// 생성자: DefaultSetupStrategy + Actuator + ConnectionConfig를 한 번에 주입하여
		/// 생성 시점에 Repository, VisionProcessor, Context를 미리 조립합니다.
		/// 상태머신 모드: 생성 후 RunSequence() 호출
		/// 직접 모드: 생성 후 ExecuteDirectVisionCommand() 호출
		/// </summary>
		/// <param name="strategy">DefaultSetupStrategy 구현체</param>
		/// <param name="connectionConfig">Vision 서버 연결 설정 (기본값 있음)</param>
		/// <param name="actuator">액추에이터 (직접 모드에서는 기본값 nullptr)</param>
		Orchestrator(std::shared_ptr<DefaultSetupStrategy> strategy,
		             const VisionConnectionConfig& connectionConfig = VisionConnectionConfig(),
		             IActuator* actuator = nullptr);

		

		~Orchestrator() override;

		// IResultSink 구현
		void NotifyVisionResult(int requestId, const std::vector<std::string>& results) override;

		// ---- Observer API ----
		ObserverId AddObserver(VisionResultObserver observer);
		bool RemoveObserver(ObserverId id);
		void ClearObservers();

		// ---- Sequence control (상태머신 모드) ----
/// <summary>
		/// 주입된 팩토리(IComponentSetup + ISequenceSetup)를 사용하여
		/// 컴포넌트를 조립하고 시퀀스를 실행합니다.
		/// </summary>
		bool RunSequence(IActuator* actuator,
		                 const VisionConnectionConfig& connectionConfig = VisionConnectionConfig());

		/// <summary>
		/// 생성자에서 이미 Actuator와 ConnectionConfig가 주입된 경우
		/// 인자 없이 시퀀스를 실행합니다.
		/// </summary>
		bool RunSequence();

		void StopSequence();

// Repository accessor
		DataRepositoryPtr GetDataRepository();

		// --- [직접 모드] VisionProcessor/Repository/Context 사용 ---
		VisionProcessorPtr GetVisionProcessor() const;

		/// Context 획득 (직접 모드/상태머신 모드 모두 지원)
		VisionContextPtr GetOrCreateContext();

		/// 직접 비전 명령 실행 (상태머신 미사용)
		bool ExecuteDirectVisionCommand(VisionCommand cmd);
		bool ExecuteDirectVisionCommand(VisionCommand cmd, const StringMap& params);
		bool ExecuteDirectVisionCommand(VisionCommand cmd, const std::string& paramsName);

	protected:
		/// <summary>
		/// 공통 컴포넌트 조립 로직: factory를 통해 Repository, VP, Context 생성
		/// </summary>
		bool InitializeComponents(IComponentSetup* factory, IActuator* actuator,
		                          bool runSequence,
		                          const VisionConnectionConfig* connectionConfig = nullptr);

		/// <summary>
		/// [Refactored] 공통 컴포넌트 생성 + 선택적 시퀀스 실행
		/// InitializeComponents()와 StartSequenceFromStrategy()의 중복 로직을 통합합니다.
		/// </summary>
		/// <param name="factory">컴포넌트 생성 팩토리</param>
		/// <param name="actuator">액추에이터 (또는 nullptr)</param>
		/// <param name="connectionConfig">Vision 서버 연결 설정 (선택)</param>
		/// <param name="presetStrategy">preset 조회용 전략 (m_pCurrentStrategy에 저장)</param>
		/// <param name="runSequence">true=시퀀스 실행 모드, false=직접 모드</param>
		/// <param name="builderFactory">Builder 생성 콜백 (runSequence=true 일 때 필요)</param>
		bool CreateComponentsAndRun(IComponentSetup* factory,
		                            IActuator* actuator,
		                            const VisionConnectionConfig* connectionConfig,
		                            SequenceSetupPtr presetStrategy,
		                            bool runSequence,
		                            std::function<SequenceBuilderPtr()> builderFactory = nullptr);

		SequenceSetupPtr m_pCurrentStrategy;  // 현재 전략 (직접 모드에서 preset 조회용)
		VisionEnginePtr m_pVisionEngine;

		// --- [직접 모드] VisionProcessor/Repository 직접 보관 ---
		VisionProcessorPtr m_directVisionProcessor;
		DataRepositoryPtr     m_directDataRepository;
		VisionContextPtr         m_directContext;

		// 주입된 단일 전략 (IComponentSetup + ISequenceSetup 통합)
		std::shared_ptr<DefaultSetupStrategy> m_strategy;

		virtual VisionContextPtr CreateContext(const VisionProcessorPtr& vm, DataRepositoryPtr& repo);

		// 기본 구현: Observer 통지
		virtual void OnVisionResult(int requestId, const std::vector<std::string>& results);
		void NotifyObservers(const VisionResultPayload& payload);

	private:
		mutable std::mutex m_seqMutex;

		mutable std::mutex m_observerMutex;
		std::unordered_map<ObserverId, VisionResultObserver> m_observers;
		std::atomic<ObserverId> m_nextObserverId{ 1 };
	};

} // namespace VMF

