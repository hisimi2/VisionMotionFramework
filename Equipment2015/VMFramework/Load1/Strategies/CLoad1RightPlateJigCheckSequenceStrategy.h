#pragma once
#include "MemorySequenceStrategy.h"
#include "CLoad1PlateJigSequenceBuilder.h"

namespace VAT_LOAD1
{
	namespace Strategies
	{
        using namespace Sequence;
		class CLoad1RightPlateJigCheckSequenceStrategy : public MemorySequenceStrategy
		{
		public:
			std::string GetSequenceName() const { return "Load1RightPlateJigCheck"; }

            VMF::SequenceBuilderPtr CreateBuilder()
			{
				return VMF::SequenceBuilderPtr(new VAT_LOAD1::Sequence::CLoad1PlateJigSequenceBuilder());
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
				SetParam(params, "maxInspCount", 3);

				SetParam(params, "CameraID", 3130);
				SetParam(params, "InspectionType", 5);
				SetParam(params, "nMovePart", 5); // targetA
				SetParam(params, "bSaveImage", 0);
				SetParam(params, "nFovDirection", 0);

				auto repo = ctx->getRepository();

				int ncamIndex = 7;
				int npkgId = 1;
				int nlocateId;
				int nVisionRequestId = 5;
				double posX, posY, focusZ;

				// // 검사 좌표 추가 (Target A/B/Upper Target)

				// Target A (locateId=1)
				nlocateId = 1;
				if (repo->LoadInspInitPos(ncamIndex, nlocateId, npkgId, posX, posY, focusZ) == VMF::StorageSuccess)
				{
					AddVisionPoint(params, nlocateId, nVisionRequestId, posX, posY, focusZ);
				}

				// Target B (locateId=2)
				nlocateId = 2;
				if (repo->LoadInspInitPos(ncamIndex, nlocateId, npkgId, posX, posY, focusZ) == VMF::StorageSuccess)
				{
					AddVisionPoint(params, nlocateId, nVisionRequestId, posX, posY, focusZ);
				}

				// Upper Target (locateId=3)
				nlocateId = 3;
				if (repo->LoadInspInitPos(ncamIndex, nlocateId, npkgId, posX, posY, focusZ) == VMF::StorageSuccess)
				{
					AddVisionPoint(params, nlocateId, nVisionRequestId, posX, posY, focusZ);
				}

				ctx->SetVatParams(params);
			}
		};
	}
}
