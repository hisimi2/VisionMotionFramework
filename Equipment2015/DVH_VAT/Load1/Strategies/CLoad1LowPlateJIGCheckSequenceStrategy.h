#pragma once
#include "MemorySequenceStrategy.h"
#include "CLoad1PlateJigSequenceBuilder.h"

namespace VAT_LOAD1
{
	namespace Strategies
	{
        using namespace Sequence;
		class CLoad1LowPlateJIGCheckSequenceStrategy : public MemorySequenceStrategy
		{
		public:
			std::string GetSequenceName() const { return "Load1LowCamPlateJig"; }

            DVH_VAT::SequenceBuilderPtr CreateBuilder()
			{
				return DVH_VAT::SequenceBuilderPtr(new VAT_LOAD1::Sequence::CLoad1PlateJigSequenceBuilder());
			}

			void ConfigureParams(DVH_VAT::VatContextPtr ctx)
			{
				DVH_VAT::VatParams params;

				// 기본 파라미터
				SetParam(params, "CameraIndex", 1);
				SetParam(params, "HandID", 1);
				SetParam(params, "PkgID", 1);

				SetParam(params, "TargetDiffX", -0.045); // Upper Target<->Lower Target간 거리 값(설계수치)
				SetParam(params, "TargetDiffY", 108.477);

				SetParam(params, "PickerMaxRow", 4);
				SetParam(params, "PickerMaxCol", 8);

				SetParam(params, "standardPickerCol", 0);
				SetParam(params, "standardPickerRow", 2);

				// 소수값은 문자열로 설정
				SetParam(params, "PickerGapX_N", "15.16");
				SetParam(params, "PickerGapY_N", "15.2");
				SetParam(params, "PickerGapX_W", "15.16");
				SetParam(params, "PickerGapY_W", "27.5");

				// Low Cam Plate Jig 검사는 보정 횟수 0회로 설정 추가
				SetParam(params, "maxInspCount", 0);

				SetParam(params, "CameraID", 3028);
				SetParam(params, "InspectionType", 5);
				SetParam(params, "nMovePart", 0); // Upper Target?
				SetParam(params, "bSaveImage", 0);
				SetParam(params, "nFovDirection", 0);

				// 검사 좌표 추가
				// Low Cam PlateJig 검사시 0,0,0 설정하여 검사
				AddVisionPoint(params, 4, 5, 0.0, 0.0, 0.0);

				ctx->SetVatParams(params);
			}
		};
	}
}
