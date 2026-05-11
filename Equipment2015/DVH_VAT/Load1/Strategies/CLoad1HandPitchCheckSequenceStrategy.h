#pragma once
#include "MemorySequenceStrategy.h"

#include "CLoad1HandPitchCheckSequenceBuilder.h"

namespace VAT_LOAD1
{
	namespace Strategies
	{
        using namespace Sequence;
		class CLoad1HandPitchCheckSequenceStrategy : public MemorySequenceStrategy
		{
		public:
			std::string GetSequenceName() const { return "Load1HandPitch"; }

            VMF::SequenceBuilderPtr CreateBuilder()
			{
				return VMF::SequenceBuilderPtr(new CLoad1HandPitchCheckSequenceBuilder());
			}

			void ConfigureParams(VMF::VatContextPtr ctx)
			{
                VMF::VatParams params;

				// 기본 파라미터
				SetParam(params, "CameraIndex", 1);
				SetParam(params, "HandID", 1);
				SetParam(params, "PkgID", 1);

				SetParam(params, "TargetDiffX", -0.045); // Upper Target<->Lower Target간 거리 값(설계수치)
				SetParam(params, "TargetDiffY", 108.477);

				SetParam(params, "PickerMaxRow", 4);
				SetParam(params, "PickerMaxCol", 8);

				SetParam(params, "standardPickerCol", 0);
				SetParam(params, "standardPickerRow", 2);

				// 소수값은 문자열로 설정
				SetParam(params, "PickerGapX_N", "15.16");
				SetParam(params, "PickerGapY_N", "15.2");
				SetParam(params, "PickerGapX_W", "15.16");
				SetParam(params, "PickerGapY_W", "27.5");

				SetParam(params, "maxInspCount", 1);

				SetParam(params, "CameraID", 3028);
				SetParam(params, "InspectionType", 1);
				SetParam(params, "nMovePart", 0); // Upper Target?
				SetParam(params, "bSaveImage", 0);
				SetParam(params, "nFovDirection", 0);

				auto repo = ctx->getRepository();

				int ncamIndex = 1;
				int npkgId = 1;
				int nlocateId = 20;
				int nVisionRequestId = 5;
				double posX, posY, focusZ;

				// // 검사 좌표 추가
				if (repo->LoadInspInitPos(ncamIndex, nlocateId, npkgId, posX, posY, focusZ) == VMF::StorageSuccess)
				{
					// Offset 값 420, 420 유지
					AddVisionPoint(params, nlocateId, nVisionRequestId, posX, posY, focusZ, 420, 420);
				}

				ctx->SetVatParams(params);
			}
		};
	}
}
