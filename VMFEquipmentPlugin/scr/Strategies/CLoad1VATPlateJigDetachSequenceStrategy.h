#pragma once
#include "CLoad1VATBaseSequenceStrategy.h"
#include "Sequences\CLoad1VATPlateJigDetachSequenceBuilder.h"

namespace VAT_LOAD1
{
	namespace Strategies
	{
		class CLoad1VATPlateJigDetachSequenceStrategy : public CLoad1VATBaseSequenceStrategy
		{
		public:
			std::string GetSequenceName() const override { return "Load1PlateJigDetach"; }

			VMF::SequenceBuilderPtr CreateBuilder() override
			{
				return std::make_shared<VAT_LOAD1::Sequence::CLoad1VATPlateJigDetachSequenceBuilder>();
			}

			void ConfigureParams(VMF::VisionContextPtr ctx) override
			{
				auto repo = ctx->GetRepository();
				double x = 0.0, y = 0.0, z = 0.0;

				// ── 공통 파라미터 ──────────────────────────────────────
				RegisterCommonParam()
					.Set("HandID", 1)
					.Set("PkgID", 1)
					.Set("PickerMaxRow", 4)
					.Set("PickerMaxCol", 8)
					.Set("PickerGapX_N", "15.16")
					.Set("PickerGapY_N", "15.2")
					.Set("PickerGapX_W", "15.16")
					.Set("PickerGapY_W", "27.5")
					.Set("standardPickerCol", "0")
					.Set("standardPickerRow", "2")
					.Set("TargetDiffX", "-0.045")
					.Set("TargetDiffY", "108.477")
					.Set("bSaveImage", 0)
					.Set("nFovDirection", 0)
					.End();

				// ── LowCamFOV ? 하단카메라 Picker FOV 검사 ───────────
				{
					auto& tag = RegisterTagParam("LowCamFOV")
						.Set("CameraIndex", 1)
						.Set("maxInspCount", 1)
						.Set("VisionFOVRequestId", 7);

					if (repo && repo->LoadInspInitPos(1, 20, 1, x, y, z) == VMF::StorageSuccess)
						tag.AddPosition(20, 9, x, y, z, 420.0, 420.0);

					tag.End();
				}

				// ── LowCam ? 하단카메라 기준열 검사 ──────────────────
				{
					auto& tag = RegisterTagParam("LowCam")
						.Set("CameraIndex", 1)
						.Set("CameraID", 3028)
						.Set("InspectionType", 1)
						.Set("nMovePart", 0)
						.Set("maxInspCount", 3)
						.Set("wide_check", "1");

					if (repo && repo->LoadInspInitPos(1, 20, 1, x, y, z) == VMF::StorageSuccess)
						tag.AddPosition(20, 1, x, y, z, 420.0, 420.0);

					tag.End();
				}

				// ── UpperFOV_L ? Cam6 상단카메라 FOV (LoadTable1/2) ──
				RegisterTagParam("UpperFOV_L")
					.Set("CameraIndex", 6)
					.Set("CameraID", 3128)
					.Set("InspectionType", 9)
					.Set("nMovePart", 0)
					.Set("VisionFOVRequestId", 7)
					.AddPosition(12, 9, 0.0, 0.0, 0.0)
					.AddPosition(13, 9, 0.0, 0.0, 0.0)
					.End();

				// ── UpperFOV_R ? Cam7 상단카메라 FOV (CTray1/2) ──────
				RegisterTagParam("UpperFOV_R")
					.Set("CameraIndex", 7)
					.Set("CameraID", 3130)
					.Set("InspectionType", 9)
					.Set("nMovePart", 0)
					.Set("VisionFOVRequestId", 7)
					.AddPosition(5, 9, 0.0, 0.0, 0.0)
					.AddPosition(6, 9, 0.0, 0.0, 0.0)
					.End();

				SetParams(ctx);
			}
		};

	} // namespace Strategies
} // namespace VAT_LOAD1