#pragma once

#include "VMFramework/MemorySequenceStrategy.h"
#include "VMFramework/Load1/Sequences/CPickPlaceSequenceBuilder.h"

namespace VMF_Load1
{
 namespace Strategies
 {
 class CPickPlaceSequenceStrategy : public MemorySequenceStrategy
 {
 public:
 std::string GetSequenceName() const override
 {
 return "Load1PickPlace";
 }

 VMF::SequenceBuilderPtr CreateBuilder() override
 {
 return VMF::SequenceBuilderPtr(new VMF_Load1::Sequence::CPickPlaceSequenceBuilder());
 }

 void ConfigureParams(VMF::VatContextPtr ctx) override
 {
 VMF::VatParams params;

 SetParam(params, "PickX",100.0);
 SetParam(params, "PickY",200.0);
 SetParam(params, "PickZ", -10.0);

 SetParam(params, "PlaceX",300.0);
 SetParam(params, "PlaceY",150.0);
 SetParam(params, "PlaceZ", -12.0);

 SetParam(params, "SafeZ",0.0);
 SetParam(params, "MoveTimeoutMs",3000.0);
 SetParam(params, "ClampIndex",0.0);
 SetParam(params, "VacuumIndex",0.0);

 ctx->SetVatParams(params);
 }
 };
 }
}
