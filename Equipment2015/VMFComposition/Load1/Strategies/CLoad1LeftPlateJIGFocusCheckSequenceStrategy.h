#pragma once
#include "VMFComposition/MemorySequenceStrategy.h"
#include "VMFComposition/Load1/Sequences/CLoad1ZFocusSequenceBuilder.h"

namespace VMF_Load1
{
    using namespace VMF;
    using namespace Sequence;

    class CLoad1LeftPlateJIGFocusCheckSequenceStrategy : public MemorySequenceStrategy
	{
	public:
		std::string GetSequenceName() const { return "Load1ZFocus"; }

		SequenceBuilderPtr CreateBuilder()
		{
			return std::make_shared<CLoad1ZFocusSequenceBuilder>();
		}

		void ConfigureParams(VMF::VisionContextPtr ctx)
		{
            VisionParams params;

			// 헬퍼 함수를 사용하여 파라미터 설정 간소화
			SetParam(params, "CameraIndex", 6);
			SetParam(params, "HandID", 1);
			SetParam(params, "PkgID", 1);
			SetParam(params, "PickerMaxRow", 4);
			SetParam(params, "PickerMaxCol", 8);

			SetParam(params, "CameraID", 3128);
			SetParam(params, "InspectionType", 6);
			SetParam(params, "nMovePart", 0); // 5 : tagret_jig_L
			SetParam(params, "bSaveImage", 0);
			SetParam(params, "nFovDirection", 0);

            auto repo = ctx->GetRepository();

			int ncamIndex = 6;
			int nlocateId = 3;
			int npkgId = 1;
			int nVisionRequestId = 6;
			double posX, posY, focusZ;

			// // 검사 좌표 추가 (locateId=3, requestId=6)
			if (repo->LoadInspInitPos(ncamIndex, nlocateId, npkgId, posX, posY, focusZ) == VMF::StorageSuccess)
			{
				AddVisionPoint(params, nlocateId, nVisionRequestId, posX, posY, focusZ);
			}

			ctx->SetVisionParams(params);
		}
	};
}
