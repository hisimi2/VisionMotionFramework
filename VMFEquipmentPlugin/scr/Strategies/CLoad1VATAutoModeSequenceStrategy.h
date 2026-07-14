#pragma once
#include "CLoad1VATBaseSequenceStrategy.h"
#include "Sequences\CLoad1VATAutoModeSequenceBuilder.h"

namespace VAT_LOAD1
{
	namespace Strategies
	{
		class CLoad1VATAutoModeSequenceStrategy : public CLoad1VATBaseSequenceStrategy
		{
		public:
			std::string GetSequenceName() const override { return "Load1AutoMode"; }

			VMF::SequenceBuilderPtr CreateBuilder() override
			{
				return std::make_shared<VAT_LOAD1::Sequence::CLoad1VATAutoModeSequenceBuilder>();
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
					.End();

				// ── HandPitch ? 하단카메라 (CameraIndex=1) ─────────────
				{
					auto& tag = RegisterTagParam("HandPitch")
						.Set("CameraIndex", 1)
						.Set("CameraID", 3028)
						.Set("InspectionType", 1)
						.Set("nMovePart", 0)
						.Set("bSaveImage", 0)
						.Set("nFovDirection", 0)
						.Set("maxInspCount", 1);

					if (repo && repo->LoadInspInitPos(1, 20, 1, x, y, z) == VMF::StorageSuccess)
						tag.AddPosition(20, 5, x, y, z, 420.0, 420.0);

					tag.End();
				}

				// ── LeftCam ? 상단 좌 카메라 (CameraIndex=6) ──────────
				{
					auto& tag = RegisterTagParam("LeftCam")
						.Set("CameraIndex", 6)
						.Set("CameraID", 3128)
						.Set("InspectionType", 5)
						.Set("nMovePart", 2)
						.Set("bSaveImage", 0)
						.Set("nFovDirection", 0)
						.Set("maxInspCount", 3);

					if (repo && repo->LoadInspInitPos(6, 12, 1, x, y, z) == VMF::StorageSuccess)
						tag.AddPosition(12, 4, x, y, z, -65.0, -65.0);
					if (repo && repo->LoadInspInitPos(6, 13, 1, x, y, z) == VMF::StorageSuccess)
						tag.AddPosition(13, 4, x, y, z, -65.0, -65.0);
					if (repo && repo->LoadInspInitPos(6, 5, 1, x, y, z) == VMF::StorageSuccess)
						tag.AddPosition(5, 2, x, y, z, -65.0, -65.0);

					tag.End();
				}

				// ── RightCam ? 상단 우 카메라 (CameraIndex=7) ─────────
				{
					auto& tag = RegisterTagParam("RightCam")
						.Set("CameraIndex", 7)
						.Set("CameraID", 3130)
						.Set("InspectionType", 5)
						.Set("nMovePart", 1)
						.Set("bSaveImage", 0)
						.Set("nFovDirection", 0)
						.Set("maxInspCount", 3)
						.Set("VisionFOVRequestId", 7);

					if (repo && repo->LoadInspInitPos(7, 6, 1, x, y, z) == VMF::StorageSuccess)
						tag.AddPosition(6, 2, x, y, z, -65.0, -65.0);

					tag.End();
				}

				SetParams(ctx);
			}
		};

	} // namespace Strategies
} // namespace VAT_LOAD1