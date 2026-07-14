#pragma once
#include "CLoad1VATBaseSequenceStrategy.h"
#include "Sequences\CLoad1VATPlateJigAttachSequenceBuilder.h"

namespace VAT_LOAD1
{
	namespace Strategies
	{
		class CLoad1VATPlateJigAttachSequenceStrategy : public CLoad1VATBaseSequenceStrategy
		{
		public:
			std::string GetSequenceName() const override { return "Load1PlateJigAttach"; }

			VMF::SequenceBuilderPtr CreateBuilder() override
			{
				return std::make_shared<VAT_LOAD1::Sequence::CLoad1VATPlateJigAttachSequenceBuilder>();
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
					.Set("bSaveImage", 0)
					.Set("nFovDirection", 0)
					.End();

				// ── ZFocus_L ? Cam6 ZFocus 검사 ───────────────────────
				{
					auto& tag = RegisterTagParam("ZFocus_L")
						.Set("CameraIndex", 6)
						.Set("CameraID", 3128)
						.Set("InspectionType", 6)
						.Set("nMovePart", 0);

					if (repo && repo->LoadInspInitPos(6, 3, 1, x, y, z) == VMF::StorageSuccess)
						tag.AddPosition(3, 6, x, y, z);

					tag.End();
				}

				// ── ZFocus_R ? Cam7 ZFocus 검사 ───────────────────────
				{
					auto& tag = RegisterTagParam("ZFocus_R")
						.Set("CameraIndex", 7)
						.Set("CameraID", 3130)
						.Set("InspectionType", 6)
						.Set("nMovePart", 0);

					if (repo && repo->LoadInspInitPos(7, 3, 1, x, y, z) == VMF::StorageSuccess)
						tag.AddPosition(3, 6, x, y, z);

					tag.End();
				}

				// ── JigFOV_L ? Cam6 PlateJig FOV 검사 ────────────────
				{
					auto& tag = RegisterTagParam("JigFOV_L")
						.Set("CameraIndex", 6)
						.Set("CameraID", 3128)
						.Set("InspectionType", 9)
						.Set("nMovePart", 0)
						.Set("maxInspCount", 1)
						.Set("VisionFOVRequestId", 7);

					if (repo && repo->LoadInspInitPos(6, 3, 1, x, y, z) == VMF::StorageSuccess)
						tag.AddPosition(3, 9, x, y, z, -65.0, -65.0);

					tag.End();
				}

				// ── JigFOV_R ? Cam7 PlateJig FOV 검사 ────────────────
				{
					auto& tag = RegisterTagParam("JigFOV_R")
						.Set("CameraIndex", 7)
						.Set("CameraID", 3130)
						.Set("InspectionType", 9)
						.Set("nMovePart", 0)
						.Set("maxInspCount", 1)
						.Set("VisionFOVRequestId", 7);

					if (repo && repo->LoadInspInitPos(7, 3, 1, x, y, z) == VMF::StorageSuccess)
						tag.AddPosition(3, 9, x, y, z);

					tag.End();
				}

				// ── JigCheck_L ? Cam6 PlateJig Check ─────────────────
				{
					auto& tag = RegisterTagParam("JigCheck_L")
						.Set("CameraIndex", 6)
						.Set("CameraID", 3128)
						.Set("InspectionType", 5)
						.Set("nMovePart", 5)
						.Set("maxInspCount", 3);

					if (repo && repo->LoadInspInitPos(6, 1, 1, x, y, z) == VMF::StorageSuccess)
						tag.AddPosition(1, 5, x, y, z);
					if (repo && repo->LoadInspInitPos(6, 2, 1, x, y, z) == VMF::StorageSuccess)
						tag.AddPosition(2, 5, x, y, z);
					if (repo && repo->LoadInspInitPos(6, 3, 1, x, y, z) == VMF::StorageSuccess)
						tag.AddPosition(3, 5, x, y, z);

					tag.End();
				}

				// ── JigCheck_R ? Cam7 PlateJig Check ─────────────────
				{
					auto& tag = RegisterTagParam("JigCheck_R")
						.Set("CameraIndex", 7)
						.Set("CameraID", 3130)
						.Set("InspectionType", 5)
						.Set("nMovePart", 5)
						.Set("maxInspCount", 3);

					if (repo && repo->LoadInspInitPos(7, 1, 1, x, y, z) == VMF::StorageSuccess)
						tag.AddPosition(1, 5, x, y, z);
					if (repo && repo->LoadInspInitPos(7, 2, 1, x, y, z) == VMF::StorageSuccess)
						tag.AddPosition(2, 5, x, y, z);
					if (repo && repo->LoadInspInitPos(7, 3, 1, x, y, z) == VMF::StorageSuccess)
						tag.AddPosition(3, 5, x, y, z);

					tag.End();
				}

				SetParams(ctx);
			}
		};

	} // namespace Strategies
} // namespace VAT_LOAD1