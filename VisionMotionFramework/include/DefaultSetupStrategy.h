#pragma once

#include "VMF_API.h"
#include "ComponentSetupBase.h"
#include "ISequenceSetup.h"
#include <string>

namespace VMF
{
   
    class VMF_API DefaultSetupStrategy : public ComponentSetupBase, public ISequenceSetup
    {
    public:
        DefaultSetupStrategy() = default;
        virtual ~DefaultSetupStrategy() = default;

        // ISequenceSetup 순수 가상 함수 — 파생 클래스에서 반드시 구현
        virtual std::string GetSequenceName() const override = 0;
        virtual SequenceBuilderPtr CreateBuilder() override = 0;
    };
}
