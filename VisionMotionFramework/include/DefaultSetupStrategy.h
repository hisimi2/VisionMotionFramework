#pragma once

#include "VMF_API.h"
#include "ComponentSetupBase.h"
#include "ISequenceSetup.h"
#include <string>

namespace VMF
{
    /// <summary>
    /// DefaultSetupStrategy — 컴포넌트 생성 + 시퀀스 설정 통합 클래스.
    /// 
    /// ComponentSetupBase(IComponentSetup)와 ISequenceSetup을 다중 상속하여,
    /// 기존 IComponentSetup + ISequenceSetup 통합 인터페이스를 유지합니다.
    /// 
    /// [사용 목적]
    /// - VMFEquipmentPlugin과 같은 외부 코드에서 하나의 Strategy 객체로
    ///   컴포넌트(Repository, VP) 생성과 시퀀스 이름/빌더 생성을 동시에 처리합니다.
    /// 
    /// - Orchestrator에서는 IComponentSetup과 ISequenceSetup을 각각 주입받아
    ///   사용하지만, Plugin 코드의 편의를 위해 DefaultSetupStrategy를 제공합니다.
    /// 
    /// 파생 클래스에서 오버라이드해야 할 메서드:
    ///   - GetSequenceName()
    ///   - CreateBuilder()
    ///   - (선택) ConfigureParams(), GetVisionParams()
    ///   - (선택) CreateRepository(), CreateVisionProcessor()
    /// </summary>
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