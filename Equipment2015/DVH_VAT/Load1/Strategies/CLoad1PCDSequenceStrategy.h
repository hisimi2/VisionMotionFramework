#pragma once
#include "MemorySequenceStrategy.h"
#include "CLoad1PCDSequenceBuilder.h"

namespace VAT_LOAD1
{
	namespace Strategies
	{
        using namespace Sequence;
		class CLoad1PCDSequenceStrategy : public MemorySequenceStrategy
		{
		public:
			std::string GetSequenceName() const { return "Load1PCD"; }

            DVH_VAT::SequenceBuilderPtr CreateBuilder()
			{
				return DVH_VAT::SequenceBuilderPtr(new VAT_LOAD1::Sequence::CLoad1PCDSequenceBuilder());
			}

			void ConfigureParams(DVH_VAT::VatContextPtr ctx)
			{
				DVH_VAT::VatParams params;

				// 헬퍼 함수를 사용하여 파라미터 설정 간소화
				SetParam(params, "CameraIndex",     6);
				SetParam(params, "HandID",          1);
				SetParam(params, "PkgID",           1);
				SetParam(params, "maxInspCount",    3);
				SetParam(params, "pcdPitch",        250);
				SetParam(params, "scanAxis",        "X");

				SetParam(params, "CameraID",        3128);
				SetParam(params, "InspectionType",  5);
				SetParam(params, "nMovePart",       0);
				SetParam(params, "bSaveImage",      0);
				SetParam(params, "nFovDirection",   0);


				// 검사 좌표 추가
				AddVisionPoint(params,
                                6,              // locateId
                                2,              // requestId
                                35.174,         // x
                                210.085,        // y
                                0.300,          // z
                                -65,            // t1
                                -65); 

				ctx->SetVatParams(params);
			}
		};
	}
}
