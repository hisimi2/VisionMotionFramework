#pragma once

#include "VMF_API.h"
#include "ComponentSetupBase.h"
#include "ISequenceSetup.h"
#include <string>
#include <memory>

namespace VMF
{
    /// <summary>
    /// 기본 설정 전략 (Default Setup Strategy)
    /// </summary>
    /// <details>
    /// ComponentSetupBase와 ISequenceSetup을 상속받는 기본 전략 클래스입니다.
    /// 파생 클래스에서 필요한 메서드를 오버라이드하여 사용합니다.
    /// 
    /// enable_shared_from_this는 DLL 경계 문제로 인해 베이스 클래스가 아닌
    /// 파생 클래스(같은 DLL 내에서 shared_from_this()를 호출하는 쪽)에서 상속받아야 합니다.
    /// </details>
    class VMF_API DefaultSetupStrategy : public ComponentSetupBase, public ISequenceSetup
    {
    public:
        DefaultSetupStrategy() = default;
        virtual ~DefaultSetupStrategy() = default;

        // ISequenceSetup 순수 가상 함수 — 파생 클래스에서 반드시 구현
        virtual std::string GetSequenceName() const override = 0;
        virtual SequenceBuilderPtr CreateBuilder() override = 0;
        
        /// <summary>
        /// Context에 파라미터를 직접 설정합니다. (Builder 없이 직접 설정)
        /// </summary>
        /// <details>
        /// Builder 중간 단계 없이 Strategy가 직접 Context에 파라미터를 설정합니다.
        /// 4단계 리팩토링(Strategy → Context 직접 전달) 적용 시 사용됩니다.
        /// 기본 구현은 비어있으며, 파생 클래스에서 필요에 따라 오버라이드합니다.
        /// </details>
        /// <param name="ctx">파라미터를 설정할 Context</param>
        virtual void ConfigureContext(Context& ctx) override
        {
            (void)ctx;  // 기본 구현은 아무것도 하지 않음
        }
    };
}
