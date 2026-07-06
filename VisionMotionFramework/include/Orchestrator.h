#pragma once

#include "RunController.h"
#include "DefaultSetupStrategy.h"
#include "ISequenceSetup.h"
#include "IComponentSetup.h"
#include "IResultSink.h"
#include "IVisionProcessor.h"
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
		/// 기본 생성자: 팩토리 없이 Orchestrator만 생성 (직접 모드 또는 수동 주입 후 사용)
		/// </summary>
		Orchestrator();

		/// <summary>
		/// 생성자: ComponentFactory와 SequenceFactory를 주입하여 객체 조립
		/// </summary>
		/// <param name="componentFactory">IComponentSetup — Repository, VisionProcessor 생성</param>
		/// <param name="sequenceFactory">ISequenceSetup — 시퀀스 이름, Builder 생성 (직접 모드에서는 nullptr 가능)</param>
		Orchestrator(ComponentSetupPtr componentFactory, SequenceSetupPtr sequenceFactory);

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

		void StopSequence();

		// Repository accessor
		DataRepositoryPtr GetDataRepository();

		// --- [직접 모드] Strategy 없이 VisionProcessor/Repository/Context 사용 ---
		void SetVisionProcessor(VisionProcessorPtr vp);
		VisionProcessorPtr GetVisionProcessor() const;
		void SetDataRepository(DataRepositoryPtr repo);

		/// Context 획득 (직접 모드/상태머신 모드 모두 지원)
		VisionContextPtr GetOrCreateContext();

		/// 직접 비전 명령 실행 (상태머신 미사용)
		bool ExecuteDirectVisionCommand(VisionCommand cmd);
		bool ExecuteDirectVisionCommand(VisionCommand cmd, const StringMap& params);
		bool ExecuteDirectVisionCommand(VisionCommand cmd, const std::string& paramsName);

/// <summary>
		/// [Plugin] 외부 Plugin DLL에서 생성된 DefaultSetupStrategy를 받아 시퀀스를 실행합니다.
		/// DefaultSetupStrategy는 IComponentSetup + ISequenceSetup을 통합합니다.
		/// </summary>
		/// <param name="strategy">Plugin DLL에서 생성한 Strategy 객체 (DefaultSetupStrategy*)</param>
		/// <param name="actuator">액추에이터 (또는 nullptr)</param>
		/// <param name="connectionConfig">Vision 서버 연결 설정 (선택, 비워두면 기본 모드)</param>
		bool StartSequenceFromStrategy(
			std::shared_ptr<DefaultSetupStrategy> strategy,
			IActuator* actuator,
			const VisionConnectionConfig& connectionConfig = VisionConnectionConfig());

		/// <summary>
		/// [Direct Mode] 외부 DLL에서 생성된 DefaultSetupStrategy를 받아
		/// 직접 모드로 컴포넌트를 초기화합니다. (시퀀스 실행 없음)
		/// </summary>
		bool InitializeDirect(std::shared_ptr<DefaultSetupStrategy> strategy);

	protected:
		/// <summary>
		/// 공통 컴포넌트 조립 로직: factory를 통해 Repository, VP, Context 생성
		/// </summary>
		bool InitializeComponents(IComponentSetup* factory, IActuator* actuator,
		                          bool runSequence,
		                          const VisionConnectionConfig* connectionConfig = nullptr);

		SequenceSetupPtr m_pCurrentStrategy;  // 현재 전략 (직접 모드에서 preset 조회용)
		VisionEnginePtr m_pVisionEngine;

		// --- [직접 모드] VisionProcessor/Repository 직접 보관 ---
		VisionProcessorPtr m_directVisionProcessor;
		DataRepositoryPtr     m_directDataRepository;
		VisionContextPtr         m_directContext;

		// 주입된 팩토리
		ComponentSetupPtr m_componentFactory;
		SequenceSetupPtr  m_sequenceFactory;

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

