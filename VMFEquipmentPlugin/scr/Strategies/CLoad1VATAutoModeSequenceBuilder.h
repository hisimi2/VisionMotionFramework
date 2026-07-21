#pragma once
#include "SequenceBuilderBase.h"
#include "VMFEquipmentPluginExport.h"

namespace VMF_PLUGIN
{
	// ----------------------------------------------------------------
	// CLoad1VATAutoModeSequenceBuilder
	//
	// 3개 그룹의 Task를 하나의 Sequence로 구성.
	// 각 Task는 SetTaskParams()로 면별 파라미터를 직접 주입.
	// ----------------------------------------------------------------
	class VMF_PLUGIN_API CLoad1VATAutoModeSequenceBuilder : public VMF::SequenceBuilderBase
	{
	public:
		std::string GetSequenceName() const { return "Load1AutoMode"; }

	protected:
        VMF::SequencePtr BuildSequence(const std::string& sequenceName) override;
	};
}
