#pragma once

#include "VMF_API.h"
#include "ComponentSetupBase.h"
#include "ISequenceSetup.h"
#include <string>

namespace VMF
{
    /// <summary>
    /// DefaultSetupStrategy — IComponentSetup(ComponentSetupBase) + ISequenceSetup 통합 클래스.
    /// 
    /// ComponentSetupBase를 상속받아 Repository, VisionProcessor 생성 기능을 제공하고,
    /// ISequenceSetup을 상속받아 시퀀스 이름 및 Builder 생성을 파생 클래스에 위임합니다.
    /// 
    /// [상속 구조 단순화]
    /// - IStrategySetup 인터페이스 제거 (다이아몬드 상속 문제 해결)
    /// - DefaultSetupStrategy는 ComponentSetupBase와 ISequenceSetup을 직접 상속
    /// - Orchestrator는 IStrategySetup 대신 DefaultSetupStrategy를 직접 사용
    /// 
    /// [파생 클래스 책임]
    /// - GetSequenceName(): 실행할 시퀀스 이름 반환
    /// - CreateBuilder(): 시퀀스 빌더 생성
    /// - (선택) CreateRepository(), CreateVisionProcessor(), ConfigureParams() 오버라이드
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
