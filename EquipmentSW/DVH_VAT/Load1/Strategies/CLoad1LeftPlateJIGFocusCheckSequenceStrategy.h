#pragma once
#include "MemorySequenceStrategy.h"

#include "CLoad1ZFocusSequenceBuilder.h"

namespace VAT_LOAD1
{
	namespace Strategies
	{
        using namespace Sequence;
		class CLoad1LeftPlateJIGFocusCheckSequenceStrategy : public MemorySequenceStrategy
		{
		public:
			std::string GetSequenceName() const { return "Load1ZFocus"; }

			DVH_VAT::SequenceBuilderPtr CreateBuilder()
			{
				return boost::make_shared<CLoad1ZFocusSequenceBuilder>();
			}

			void ConfigureParams(DVH_VAT::VatContextPtr ctx)
			{
                DVH_VAT::VatParams params;

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

				auto repo = ctx->getRepository();

				int ncamIndex = 6;
				int nlocateId = 3;
				int npkgId = 1;
				int nVisionRequestId = 6;
				double posX, posY, focusZ;

				// // 검사 좌표 추가 (locateId=3, requestId=6)
				if (repo->LoadInspInitPos(ncamIndex, nlocateId, npkgId, posX, posY, focusZ) == DVH_VAT::StorageSuccess)
				{
					AddVisionPoint(params, nlocateId, nVisionRequestId, posX, posY, focusZ);
				}

				ctx->SetVatParams(params);
			}
		};
	}
}
