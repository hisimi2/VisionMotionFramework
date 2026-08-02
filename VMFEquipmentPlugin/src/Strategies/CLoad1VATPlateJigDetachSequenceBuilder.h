#pragma once
#include "SequenceBuilderBase.h"

namespace VMF_PLUGIN
{
	// ----------------------------------------------------------------
	// CLoad1VATPlateJigDetachSequenceBuilder
	//
	// PlateJig 탈착 시퀀스:
	//   [LowCamFOV]  하단카메라 FOV 검사 (Cam1, Picker FOV)
	//   [LowCam]     하단카메라 기준열 검사 (Cam1, Picker 위치)
	//   [UpperFOV_L] 상단카메라 FOV 검사 (Cam6, LoadTable1/2)
	//   [UpperFOV_R] 상단카메라 FOV 검사 (Cam7, CTray1/2)
	// ----------------------------------------------------------------
	class CLoad1VATPlateJigDetachSequenceBuilder : public VMF::SequenceBuilderBase
	{
	public:
		std::string GetSequenceName() const { return "Load1PlateJigDetach"; }

	protected:
        VMF::SequencePtr BuildSequence(const std::string& sequenceName) override;
	};
}
