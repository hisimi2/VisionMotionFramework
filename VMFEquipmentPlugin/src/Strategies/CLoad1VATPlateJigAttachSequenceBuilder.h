#pragma once
#include "SequenceBuilderBase.h"


namespace VMF_PLUGIN
{

	// ----------------------------------------------------------------
	// CLoad1VATPlateJigAttachSequenceBuilder
	//
	// PlateJig 장착 시퀀스:
	//   [ZFocus_L]   Cam6 ZFocus 검사
	//   [ZFocus_R]   Cam7 ZFocus 검사
	//   [JigFOV_L]   Cam6 PlateJig FOV 검사
	//   [JigFOV_R]   Cam7 PlateJig FOV 검사
	//   [JigCheck_L] Cam6 PlateJig Check (TargetA/B/Upper)
	//   [JigCheck_R] Cam7 PlateJig Check (TargetA/B/Upper)
	// ----------------------------------------------------------------
	class CLoad1VATPlateJigAttachSequenceBuilder : public VMF::SequenceBuilderBase
	{
	public:
		std::string GetSequenceName() const { return "Load1PlateJigAttach"; }

	protected:
        VMF::SequencePtr BuildSequence(const std::string& sequenceName) override;
		
	};
}
