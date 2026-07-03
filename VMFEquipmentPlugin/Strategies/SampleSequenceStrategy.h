#pragma once
#include "DefaultSetupStrategy.h"
#include "Sequences/SampleZFocusSequenceBuilder.h"
#include "VMFEquipmentPluginExport.h"

namespace VMF_Sample
{
	using namespace VMF;
	using namespace Sequence;

    /// <summary>
	/// [Sample] Focus Check Sequence 전략 클래스
	/// DefaultSetupStrategy를 상속받아 컴포넌트 생성 책임을 담당
	/// 
	/// [책임 범위]
	/// - CreateRepository(): DB 초기화 (SqliteDataRepository)
	/// - CreateVisionProcessor(): Vision 서버 연결 및 프로세서 초기화
	/// - CreateBuilder(): SampleZFocusSequenceBuilder 반환
	/// 
	/// [Builder와의 책임 분리]
	/// ╔══════════════════════════════════════════════════╗
	/// ║  SampleSequenceStrategy (Strategy)               ║
	/// ║  ├─ CreateRepository(): DB 초기화                 ║
	/// ║  ├─ CreateVisionProcessor(): Vision 연결         ║
	/// ║  └─ CreateBuilder(): Builder 반환                ║
	/// ╚══════════════════════════════════════════════════╝
	/// ╔══════════════════════════════════════════════════╗
	/// ║  SampleZFocusSequenceBuilder (Builder)           ║
	/// ║  └─ BuildSequence(): Task 조립 + params 생성       ║
	/// ║      ├─ Task 생성                                 ║
	/// ║      ├─ VisionParams 생성 및 설정                  ║
	/// ║      └─ task->SetTaskParams(params) 로 주입       ║
	/// ╚══════════════════════════════════════════════════╝
	/// 
	/// !!! 수정 가이드 !!!
	/// 1. GetSequenceName(): SampleZFocusSequenceBuilder::GetSequenceName()과 일치해야 함
	/// 2. CreateBuilder(): 장비별 시퀀스 빌더로 교체
	/// 3. ConfigureParams(): 불필요 (Builder가 직접 VisionParams 생성)
	/// </summary>
	class VMFEQUIPMENTPLUGIN_API SampleSequenceStrategy : public DefaultSetupStrategy
	{
	public:
		std::string GetSequenceName() const override { return "SampleZFocus"; }

		SequenceBuilderPtr CreateBuilder() override
		{
			return std::make_shared<SampleZFocusSequenceBuilder>();
		}

		void ConfigureParams(VMF::VisionContextPtr ctx) override
		{
			// Builder가 직접 VisionParams를 생성하여 Task에 주입하므로
			// Strategy에서 별도 params 설정이 필요 없습니다.
			(void)ctx;
		}
	};
} // namespace VMF_Sample
