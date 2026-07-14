#pragma once
#include "CLoad1VATBaseSequenceStrategy.h"
#include "Sequences\CLoad1VATAutoModeSequenceBuilder.h"

namespace VMF_PLUGIN
{
	namespace Strategies
	{
		class CLoad1VATAutoModeSequenceStrategy : public CLoad1VATBaseSequenceStrategy
		{
		public:
			std::string GetSequenceName() const override { return "Load1AutoMode"; }

			VMF::SequenceBuilderPtr CreateBuilder() override
			{
				return std::make_shared<VMF_PLUGIN::Sequence::CLoad1VATAutoModeSequenceBuilder>();
			}

			void ConfigureParams(VMF::VisionContextPtr ctx) override
			{
				// Task별 파라미터는 Builder에서 SetTaskParams()로 직접 주입
				// ConfigureParams는 Repository 초기화 등 공통 작업만 수행
				(void)ctx;
			}
		};

	} // namespace Strategies
} // namespace VMF_PLUGIN
