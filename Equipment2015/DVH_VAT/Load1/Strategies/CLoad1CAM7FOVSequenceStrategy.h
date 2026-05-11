#pragma once
#include "MemorySequenceStrategy.h"
#include "CLoad1UpperCamFOVSequenceBuilder.h"

namespace VAT_LOAD1
{
	namespace Strategies
	{
		class CLoad1CAM7FOVSequenceStrategy : public MemorySequenceStrategy
		{
		public:
			std::string GetSequenceName() const { return "Load1PlateJig"; }

            VMF::SequenceBuilderPtr CreateBuilder()
			{
				return VMF::SequenceBuilderPtr(new VAT_LOAD1::Sequence::CLoad1UpperCamFOVSequenceBuilder());
			}

			void ConfigureParams(VMF::VatContextPtr& ctx)
			{
				VMF::VatRunParams params;

				// 헬퍼 함수를 사용하여 파라미터 설정 간소화
				SetParam(params, "CameraIndex", 7);
				SetParam(params, "HandID", 1);
				SetParam(params, "PkgID", 1);
				SetParam(params, "PickerMaxRow", 4);
				SetParam(params, "PickerMaxCol", 8);
				SetParam(params, "VisionFOVRequestId", 7);

				// 검사 좌표 추가
				// CTray1 (locateId=5, requestId=9)
				AddVisionPoint(params, 5, 9, 0.0, 0.0, 0.0);

				// CTray2 (locateId=6, requestId=9)
				AddVisionPoint(params, 6, 9, 0.0, 0.0, 0.0);

				ctx->SetVatRunParams(params);
			}
		};
	}
}
