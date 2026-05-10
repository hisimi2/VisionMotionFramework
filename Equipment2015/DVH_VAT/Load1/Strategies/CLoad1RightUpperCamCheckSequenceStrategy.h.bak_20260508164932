#pragma once
#include "MemorySequenceStrategy.h"
#include "CLoad1UpperCamCheckSequenceBuilder.h"

namespace VAT_LOAD1
{
	namespace Strategies
	{
        using namespace Sequence;
		class CLoad1RightUpperCamCheckSequenceStrategy : public MemorySequenceStrategy
		{
		public:
			std::string GetSequenceName() const { return "Load1RightUpperCamCheck"; }

            DVH_VAT::SequenceBuilderPtr CreateBuilder()
			{
				return DVH_VAT::SequenceBuilderPtr(new VAT_LOAD1::Sequence::CLoad1UpperCamCheckSequenceBuilder());
			}

			void ConfigureParams(DVH_VAT::VatContextPtr ctx)
			{
				DVH_VAT::VatParams params;

				// 헬퍼 함수를 사용하여 파라미터 설정 간소화
				SetParam(params, "CameraIndex", 7);
				SetParam(params, "HandID", 1);
				SetParam(params, "PkgID", 1);
				SetParam(params, "PickerMaxRow", 4);
				SetParam(params, "PickerMaxCol", 8);
				SetParam(params, "VisionFOVRequestId", 7);
				SetParam(params, "maxInspCount", 3);
				SetParam(params, "PickerGapX_N", "15.16");
				SetParam(params, "PickerGapY_N", "15.2");

				SetParam(params, "CameraID", 3130);
				SetParam(params, "InspectionType", 5);
				SetParam(params, "nMovePart", 1); // CTray2
				SetParam(params, "bSaveImage", 0);
				SetParam(params, "nFovDirection", 0);

				auto repo = ctx->getRepository();

				int ncamIndex = 7;
				int npkgId = 1;
				int nlocateId = 6;
				int nVisionRequestId = 2;
				double posX, posY, focusZ;

				// // 검사 좌표 추가
				// CTray2 (locateId=6, requestId=2)
				if (repo->LoadInspInitPos(ncamIndex, nlocateId, npkgId, posX, posY, focusZ) == DVH_VAT::StorageSuccess)
				{
					AddVisionPoint(params, nlocateId, nVisionRequestId, posX, posY, focusZ, -65, -65);
				}

				ctx->SetVatParams(params);
			}
		};
	}
}
