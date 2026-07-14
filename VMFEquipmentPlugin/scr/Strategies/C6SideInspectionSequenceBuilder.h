#pragma once
#include "SequenceBuilderBase.h"

namespace VMF_PLUGIN
{
	// ----------------------------------------------------------------
	// C6SideInspectionSequenceBuilder
	//
	// 6면 검사 시퀀스:
	//   각 면마다 [MoveToFacePosition + Inspection] 쌍으로 구성
	//   면별 파라미터는 SetTaskParams()로 Task에 직접 주입
	// ----------------------------------------------------------------
	class C6SideInspectionSequenceBuilder : public VMF::SequenceBuilderBase
	{
	public:
		std::string GetSequenceName() const { return "6SideInspection"; }

	protected:
		VMF::SequencePtr BuildSequence(const std::string& sequenceName) override;
	};
}
