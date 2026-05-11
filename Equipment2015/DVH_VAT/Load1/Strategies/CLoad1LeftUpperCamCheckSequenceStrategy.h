#pragma once

#include "MemorySequenceStrategy.h"
#include "CLoad1UpperCamCheckSequenceBuilder.h"

namespace VAT_LOAD1
{
	namespace Strategies
	{
        using namespace Sequence;
	    class CLoad1LeftUpperCamCheckSequenceStrategy : public MemorySequenceStrategy
	    {
		public:
			std::string GetSequenceName() const { return "Load1UpperCamCheck"; }

            VMF::SequenceBuilderPtr CreateBuilder()
			{
				return VMF::SequenceBuilderPtr(new CLoad1UpperCamCheckSequenceBuilder());
			}

			void ConfigureParams(VMF::VatContextPtr ctx)
			{
                VMF::VatParams params;

				// 헬퍼 함수 사용
				SetParam(params, "CameraIndex",     6);
				SetParam(params, "HandID",          1);
				SetParam(params, "PkgID",           1);
				SetParam(params, "PickerMaxRow",    4);
				SetParam(params, "PickerMaxCol",    8);
				SetParam(params, "maxInspCount",    3);
				SetParam(params, "PickerGapX_N",    "15.16");
				SetParam(params, "PickerGapY_N",    "15.2");
				SetParam(params, "CameraID",        3128);
				SetParam(params, "InspectionType",  5);
				SetParam(params, "nMovePart",       2); // Loader table1
				SetParam(params, "bSaveImage",      0);
				SetParam(params, "nFovDirection",   0);

				// 검사 좌표 추가 (3축 파라미터 명시적 전달)
				auto repo = ctx->getRepository();

				int ncamIndex = 6;
				int npkgId = 1;
				int nlocateId;
				int nVisionRequestId;
				double posX, posY, focusZ;

				//검사 좌표 추가 
				// LoadTable1 (locateId=12, requestId=4)
				nlocateId = 12;
				nVisionRequestId = 4;
				if (repo->LoadInspInitPos(ncamIndex, nlocateId, npkgId, posX, posY, focusZ) == VMF::StorageSuccess)
				{
					AddVisionPoint(params, nlocateId, nVisionRequestId, posX, posY, focusZ, -65, -65); // Table Pos 알아야 함
				}

				// LoadTable2 (locateId=13, requestId=4)
				nlocateId = 13;
				nVisionRequestId = 4;
				if (repo->LoadInspInitPos(ncamIndex, nlocateId, npkgId, posX, posY, focusZ) == VMF::StorageSuccess)
				{
					AddVisionPoint(params, nlocateId, nVisionRequestId, posX, posY, focusZ, -65, -65);
				}

				// CTray1 (locateId=5, requestId=2)
				nlocateId = 5;
				nVisionRequestId = 2;
				if (repo->LoadInspInitPos(ncamIndex, nlocateId, npkgId, posX, posY, focusZ) == VMF::StorageSuccess)
				{
					AddVisionPoint(params, nlocateId, nVisionRequestId, posX, posY, focusZ, -65, -65);
				}

				ctx->SetVatParams(params);
			}
		};
	}
}
