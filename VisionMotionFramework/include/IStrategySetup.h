#pragma once

#include "VMF_API.h"
#include "IComponentSetup.h"
#include "ISequenceSetup.h"

namespace VMF
{
    /// <summary>
    /// IStrategySetup — IComponentSetup + ISequenceSetup 통합 인터페이스.
    /// 
    /// DefaultSetupStrategy 및 그 파생 클래스(SampleSequenceStrategy 등)는
    /// 이 인터페이스를 구현하여 하나의 객체로 컴포넌트 생성과 시퀀스 설정을
    /// 동시에 제공합니다.
    /// 
    /// [목적]
    /// - Orchestrator 생성 시 동일한 Strategy 객체를 두 번 전달하는 불합리함 제거
    ///   (기존: Orchestrator(strategy, strategy) → 개선: Orchestrator(strategy))
    /// - 타입 안전성 향상: 하나의 인터페이스로 두 역할을 명시
    /// - Plugin DLL과 Equipment App 간의 계약을 단일 인터페이스로 단순화
    /// </summary>
    class VMF_API IStrategySetup : public virtual IComponentSetup, public ISequenceSetup
    {
    public:
        ~IStrategySetup() override = default;
    };
} // namespace VMF
