#pragma once
#include "MemorySequenceStrategy.h"
#include "CLoad1ZFocusSequenceBuilder.h"

namespace VAT_LOAD1
{
	namespace Strategies
	{
        using namespace Sequence;
		class CLoad1LowCamPlateJIGFocusCheckSequenceStrategy : public MemorySequenceStrategy
		{
		public:
			std::string GetSequenceName() const { return "Load1ZFocus"; }

            VMF::SequenceBuilderPtr CreateBuilder()
			{
				return VMF::SequenceBuilderPtr(new VAT_LOAD1::Sequence::CLoad1ZFocusSequenceBuilder());
			}

			void ConfigureParams(VMF::VatContextPtr ctx)
			{
                VMF::VatParams params;

				// 기본 시퀀스 파라미터
				SetParam(params, "CameraIndex", 1);
				SetParam(params, "HandID", 1);
				SetParam(params, "PkgID", 1);
				SetParam(params, "PickerMaxRow", 4);
				SetParam(params, "PickerMaxCol", 8);

				SetParam(params, "CameraID", 3028);
				SetParam(params, "InspectionType", 6);
				SetParam(params, "nMovePart", 0);
				SetParam(params, "bSaveImage", 0);
				SetParam(params, "nFovDirection", 0);

				// Low Cam Focus 검사시 0,0,0 설정하여 검사
				AddVisionPoint(params, 4, 6, 0, 0, 0);

				ctx->SetVatParams(params);
			}
		};
	}
}
