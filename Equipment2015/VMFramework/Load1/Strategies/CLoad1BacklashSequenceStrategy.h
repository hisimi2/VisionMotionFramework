#pragma once
#include "MemorySequenceStrategy.h"

#include "CLoad1BacklashSequenceBuilder.h"

namespace VAT_LOAD1
{
	namespace Strategies
	{
        using namespace Sequence;
	    class CLoad1BacklashSequenceStrategy : public MemorySequenceStrategy
	    {
		public:
			std::string GetSequenceName() const { return "Load1Backlash"; }

			/// <summary>
            /// 이 전략에서 사용할 SequenceBuilder 객체를 생성하여 반환합니다.
			/// </summary>
            VMF::SequenceBuilderPtr CreateBuilder()
			{
				return VMF::SequenceBuilderPtr(new CLoad1BacklashSequenceBuilder());
			}

            /// <summary>
            /// 시퀀스 실행에 필요한 파라미터를 Context에 설정(주입)합니다.
            /// </summary>
			void ConfigureParams(VMF::VatContextPtr ctx)
			{
                VMF::VatParams params;

                /// 시퀀스 실행에 필요한 파라미터를 설정합니다.
				SetParam(params, "CameraIndex",     6);
				SetParam(params, "HandID",          1);
				SetParam(params, "PkgID",           1);
                SetParam(params, "scanAxis",        "X");

				SetParam(params, "StartPosX",       35.174);
				SetParam(params, "ZeroPosX",        85.174);
				SetParam(params, "EndPosX",         135.174);

				SetParam(params, "StartPosY",       210.085);
				SetParam(params, "ZeroPosY",        210.085);
				SetParam(params, "EndPosY",         210.085);

				SetParam(params, "CameraID",        3128);
				SetParam(params, "InspectionType",  5);
				SetParam(params, "nMovePart",       0);
				SetParam(params, "bSaveImage",      0);
				SetParam(params, "nFovDirection",   0);

                // 비전 검사 위치 추가 (예시)
				AddVisionPoint(params, 6, 2, 35.174, 210.085, 0.300, -65, -65); // Table Pos 알아야 함

                // 설정된 파라미터를 Context에 주입합니다.
				ctx->SetVatParams(params);
			}
		};
	}
}
