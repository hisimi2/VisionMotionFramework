#pragma once
#include "MemorySequenceStrategy.h"
#include "CLoad1UpperCamFOVSequenceBuilder.h"

namespace VAT_LOAD1
{
	namespace Strategies
	{
		class CLoad1CAM6FOVSequenceStrategy : public MemorySequenceStrategy
		{
		public:
			std::string GetSequenceName() const { return "Load1UpperCamFOV"; }

			SequenceBuilderPtr CreateBuilder()
			{
				return boost::make_shared<VAT_LOAD1::Sequence::CLoad1UpperCamFOVSequenceBuilder>();
			}

			void ConfigureParams(DVH_VAT::VatContextPtr& ctx)
			{
				DVH_VAT::VatRunParams params;

				// 헬퍼 함수를 사용하여 파라미터 설정 간소화
				SetParam(params, "CameraIndex", 6);
				SetParam(params, "HandID", 1);
				SetParam(params, "PkgID", 1);
				SetParam(params, "PickerMaxRow", 4);
				SetParam(params, "PickerMaxCol", 8);
				SetParam(params, "VisionFOVRequestId", 7);

				SetParam(params, "CameraID", 3128);
				SetParam(params, "InspectionType", 9);
				SetParam(params, "nMovePart", 0); // Upper Target?
				SetParam(params, "bSaveImage", 0);
				SetParam(params, "nFovDirection", 0);

				// 검사 좌표 추가
				// LoadTable1 (locateId=12, requestId=9)
				AddVisionPoint(params, 12, 9, 0.0, 0.0, 0.0);

				// LoadTable2 (locateId=13, requestId=9)
				AddVisionPoint(params, 13, 9, 0.0, 0.0, 0.0);

				ctx->SetVatRunParams(params);
			}
		};
	}
}
