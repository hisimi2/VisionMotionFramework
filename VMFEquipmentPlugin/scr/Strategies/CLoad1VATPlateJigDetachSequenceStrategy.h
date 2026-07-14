#pragma once
#include "CLoad1VATBaseSequenceStrategy.h"
#include "Sequences\CLoad1VATPlateJigDetachSequenceBuilder.h"

namespace VMF_PLUGIN
{
	namespace Strategies
	{
		class CLoad1VATPlateJigDetachSequenceStrategy : public CLoad1VATBaseSequenceStrategy
		{
		public:
			std::string GetSequenceName() const override { return "Load1PlateJigDetach"; }

			VMF::SequenceBuilderPtr CreateBuilder() override
			{
				return std::make_shared<VMF_PLUGIN::Sequence::CLoad1VATPlateJigDetachSequenceBuilder>();
			}

			void ConfigureParams(VMF::VisionContextPtr ctx) override
			{
				// Task별 파라미터는 Builder에서 SetTaskParams()로 직접 주입
				(void)ctx;
			}
		};

	} // namespace Strategies
} // namespace VMF_PLUGIN