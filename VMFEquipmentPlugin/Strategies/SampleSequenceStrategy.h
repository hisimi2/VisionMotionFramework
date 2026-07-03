#pragma once
#include "ComponentSetupBase.h"
#include "Sequences/SampleZFocusSequenceBuilder.h"
#include "VMFEquipmentPluginExport.h"

namespace VMF_Sample
{
	using namespace VMF;
	using namespace Sequence;

	/// <summary>
	/// [Sample] Focus Check Sequence 전략 클래스
	/// ComponentSetupBase를 상속받아 컴포넌트 생성 책임을 담당
	/// 
	/// [책임 범위]
	/// - CreateRepository(): DB 초기화 (SqliteDataRepository) — ComponentSetupBase 기본 구현 사용
	/// - CreateVisionProcessor(): Vision 서버 연결 및 프로세서 초기화 — ComponentSetupBase 기본 구현 사용
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
	/// 1. GetSequenceName(): 실행할 시퀀스 이름 반환
	/// 2. CreateBuilder(): 장비별 시퀀스 빌더로 교체
	/// </summary>
	class VMFEQUIPMENTPLUGIN_API SampleSequenceStrategy : public VMF::ComponentSetupBase
	{
	public:
		std::string GetSequenceName() const override { return "SampleZFocus"; }

		SequenceBuilderPtr CreateBuilder() override
		{
			return std::make_shared<SampleZFocusSequenceBuilder>();
		}
	};
} // namespace VMF_Sample
