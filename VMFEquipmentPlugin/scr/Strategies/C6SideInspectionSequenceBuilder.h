#pragma once
#include "SequenceBuilderBase.h"

namespace VMF_PLUGIN
{
	// ----------------------------------------------------------------
	// C6SideInspectionSequenceBuilder
	//
	// 6면 검사 시퀀스:
	//   각 면마다 [MoveToFacePosition + Inspection] 쌍으로 구성
	//   태그로 면별 파라미터 분리:
	//     Face_Left / Face_Right / Face_Front
	//     Face_Top  / Face_Rear  / Face_Bottom
	// ----------------------------------------------------------------
	class C6SideInspectionSequenceBuilder : public VMF::SequenceBuilderBase
	{
	public:
		std::string GetSequenceName() const { return "6SideInspection"; }

	protected:
        VMF::SequencePtr BuildSequence(const std::string& sequenceName) override;
	};
}
