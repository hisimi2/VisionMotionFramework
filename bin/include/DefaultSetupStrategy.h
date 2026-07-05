#pragma once

#include "VMF_API.h"
#include "ComponentSetupBase.h"
#include "IStrategySetup.h"
#include <string>

namespace VMF
{
    /// <summary>
    /// DefaultSetupStrategy — 컴포넌트 생성 + 시퀀스 설정 통합 클래스.
    /// 
    /// ComponentSetupBase(IComponentSetup)와 IStrategySetup(IComponentSetup+ISequenceSetup)을
    /// 다중 상속하여, 기존 IComponentSetup + IStrategySetup 통합 인터페이스를 유지합니다.
    /// 
    /// [사용 목적]
    /// - VMFEquipmentPlugin과 같은 외부 코드에서 하나의 Strategy 객체로
    ///   컴포넌트(Repository, VP) 생성과 시퀀스 이름/빌더 생성을 동시에 처리합니다.
    /// 
    /// - Orchestrator에서는 IStrategySetup 하나만 주입받아도
    ///   IComponentSetup과 ISequenceSetup을 모두 사용할 수 있습니다.
    /// 
    /// 파생 클래스에서 오버라이드해야 할 메서드:
    ///   - GetSequenceName()
    ///   - CreateBuilder()
    ///   - (선택) ConfigureParams(), GetVisionParams()
    ///   - (선택) CreateRepository(), CreateVisionProcessor()
    /// </summary>
    class VMF_API DefaultSetupStrategy : public ComponentSetupBase, public IStrategySetup
    {
    public:
        DefaultSetupStrategy() = default;
        virtual ~DefaultSetupStrategy() = default;

        // ISequenceSetup 순수 가상 함수 — 파생 클래스에서 반드시 구현
        virtual std::string GetSequenceName() const override = 0;
        virtual SequenceBuilderPtr CreateBuilder() override = 0;
    };
}