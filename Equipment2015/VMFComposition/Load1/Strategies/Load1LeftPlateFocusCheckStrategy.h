#pragma once
#include "VMFComposition/DefaultSetupStrategy.h"
#include "VMFComposition/Load1/Sequences/CLoad1ZFocusSequenceBuilder.h"

namespace VMF_Load1
{
    using namespace VMF;
    using namespace Sequence;

    class Load1LeftPlateFocusCheckStrategy : public DefaultSetupStrategy
	{
	public:
		std::string GetSequenceName() const { return "Load1ZFocus"; }

		SequenceBuilderPtr CreateBuilder()
		{
			return std::make_shared<CLoad1ZFocusSequenceBuilder>();
		}

        void ConfigureParams(VMF::VisionContextPtr ctx) override
        {
            VisionParams params;

            SetParam(params, "HandID", 1);
            SetParam(params, "PkgID", 1);

            auto repo = ctx->GetRepository();
            const int ncamIndex = 6;
            const int nlocateId = 3;
            const int npkgId = 1;
            const int nVisionRequestId = 6;
            double posX = 0.0, posY = 0.0, focusZ = 0.0;

            if (repo &&
                repo->LoadInspInitPos(ncamIndex, nlocateId, npkgId,
                    posX, posY, focusZ) == VMF::StorageSuccess)
            {
                AddVisionPoint(params, nlocateId, nVisionRequestId,
                    posX, posY, focusZ);
            }

            ctx->SetVisionParams(params);
        }

        // 카메라/검사별 Preset 파라미터 반환 
        StringMap GetVisionParams(const std::string& presetName) const override
        {
            if (presetName == "Cam6Focus")
            {
                StringMap p;
                p["CameraIndex"] = "6";
                p["InspectionType"] = "6";
                p["CameraID"] = "3128";
                return p;
            }
            if (presetName == "PLVI")
            {
                StringMap p;
                p["CameraIndex"] = "3";
                p["InspectionType"] = "2";
                return p;
            }
            return StringMap();
        }
	};
}
