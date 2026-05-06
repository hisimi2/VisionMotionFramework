#pragma once

#include "MemorySequenceStrategy.h"
#include "CLoad1PlateJigFOVSequenceBuilder.h"

namespace VAT_LOAD1
{
	namespace Strategies
	{
        using namespace Sequence;
		class CLoad1LeftPlateJigFOVCheckSequenceStrategy : public MemorySequenceStrategy
		{
		public:
			std::string GetSequenceName() const { return "Load1PlateJigFOV"; }

            DVH_VAT::SequenceBuilderPtr CreateBuilder()
			{
				return boost::make_shared<CLoad1PlateJigFOVSequenceBuilder>();
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
				SetParam(params, "VisionFOVRequestId", 7);
				SetParam(params, "maxInspCount", 1);

				SetParam(params, "CameraID", 3128);
				SetParam(params, "InspectionType", 9);
				SetParam(params, "nMovePart", 0); // Upper Target?
				SetParam(params, "bSaveImage", 0);
				SetParam(params, "nFovDirection", 0);

				auto repo = ctx->getRepository();

				int ncamIndex = 6;
				int nlocateId = 3;
				int npkgId = 1;
				int nVisionRequestId = 9;
				double posX, posY, focusZ;

				if (repo->LoadInspInitPos(ncamIndex, nlocateId, npkgId, posX, posY, focusZ) == DVH_VAT::StorageSuccess)
				{
					// Backlash 위치 좌표
					AddVisionPoint(params, nlocateId, nVisionRequestId, posX, posY, focusZ, -65, -65); // Table Pos 알아야 함
				}

				ctx->SetVatParams(params);
			}
		};
	}
}
