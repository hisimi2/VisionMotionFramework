#pragma once
#include "MemorySequenceStrategy.h"
#include "CLoad1ZFocusSequenceBuilder.h"

namespace VAT_LOAD1
{
	namespace Strategies
	{
        using namespace Sequence;
		class CLoad1RightPlateJIGFocusCheckSequenceStrategy : public MemorySequenceStrategy
		{
		public:
			std::string GetSequenceName() const { return "Load1RightPlateJIGFocusCheck"; }

            VMF::SequenceBuilderPtr CreateBuilder()
			{
				return VMF::SequenceBuilderPtr(new VAT_LOAD1::Sequence::CLoad1ZFocusSequenceBuilder());
			}

			void ConfigureParams(VMF::VatContextPtr ctx)
			{
                VMF::VatParams params;

				// 기본 시퀀스 파라미터
				SetParam(params, "CameraIndex", 7);
				SetParam(params, "HandID", 1);
				SetParam(params, "PkgID", 1);
				SetParam(params, "PickerMaxRow", 4);
				SetParam(params, "PickerMaxCol", 8);

				SetParam(params, "CameraID", 3130);
				SetParam(params, "InspectionType", 6);
				SetParam(params, "nMovePart", 0); // 6 : tagret_jig_R
				SetParam(params, "bSaveImage", 0);
				SetParam(params, "nFovDirection", 0);

				auto repo = ctx->getRepository();

				int ncamIndex = 7;
				int npkgId = 1;
				int nlocateId = 3;
				int nVisionRequestId = 6;
				double posX, posY, focusZ;

				// // 검사 좌표 추가 (locateId=3, requestId=6)
				if (repo->LoadInspInitPos(ncamIndex, nlocateId, npkgId, posX, posY, focusZ) == VMF::StorageSuccess)
				{
					AddVisionPoint(params, nlocateId, nVisionRequestId, posX, posY, focusZ);
				}

				ctx->SetVatParams(params);
			}
		};
	}
}
