#pragma once
#include "MemorySequenceStrategy.h"
#include "CLoad1PlateJigFOVSequenceBuilder.h"

namespace VAT_LOAD1
{
	namespace Strategies
	{
        using namespace Sequence;
		class CLoad1RightJigFOVCheckSequenceStrategy : public MemorySequenceStrategy
		{
		public:
			std::string GetSequenceName() const { return "Load1RightJigFOVCheck"; }

            VMF::SequenceBuilderPtr CreateBuilder()
			{
				return VMF::SequenceBuilderPtr(new VAT_LOAD1::Sequence::CLoad1PlateJigFOVSequenceBuilder());
			}

			void ConfigureParams(VMF::VatContextPtr ctx)
			{
				VMF::VatParams params;

				// 헬퍼 함수를 사용하여 파라미터 설정 간소화
				SetParam(params, "CameraIndex", 7);
				SetParam(params, "HandID", 1);
				SetParam(params, "PkgID", 1);
				SetParam(params, "PickerMaxRow", 4);
				SetParam(params, "PickerMaxCol", 8);
				SetParam(params, "VisionFOVRequestId", 7);
				SetParam(params, "maxInspCount", 1);


				SetParam(params, "CameraID", 3130);
				SetParam(params, "InspectionType", 9);
				SetParam(params, "nMovePart", 0); // Upper Target?
				SetParam(params, "bSaveImage", 0);
				SetParam(params, "nFovDirection", 0);

				auto repo = ctx->getRepository();

				int ncamIndex = 7;
				int npkgId = 1;
				int nlocateId = 3;
				int nVisionRequestId = 9;
				double posX, posY, focusZ;

				// // 검사 좌표 추가 (Upper Target, locateId=3, requestId=9)
				if (repo->LoadInspInitPos(ncamIndex, nlocateId, npkgId, posX, posY, focusZ) == VMF::StorageSuccess)
				{
					AddVisionPoint(params, nlocateId, nVisionRequestId, posX, posY, focusZ);
				}

				ctx->SetVatParams(params);
			}
		};
	}
}
