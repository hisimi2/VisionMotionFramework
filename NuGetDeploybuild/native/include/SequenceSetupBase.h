#pragma once
#include "VMF_API.h"
#include "ComponentSetupBase.h"
#include "ISequenceSetup.h"

namespace VMF
{
    /// <summary>
    /// 시퀀스 실행에 필요한 컴포넌트(VP, Repo) 조립 + 시퀀스(Sequence) 이름/빌더를 함께 정의하는 기본 클래스.
    /// ComponentSetupBase(컴포넌트 생성) + ISequenceSetup(시퀀스 생성)을 결합합니다.
    /// 상태머신 모드에서 사용합니다. 직접 모드에서는 ComponentSetupBase 만으로 충분합니다.
    /// </summary>
    class VMF_API SequenceSetupBase : public ComponentSetupBase, public ISequenceSetup
    {
    public:
        SequenceSetupBase();
        ~SequenceSetupBase() override = default;

        // ISequenceSetup 인터페이스 (파생 클래스에서 구현)
        // std::string GetSequenceName() const override = 0;
        // SequenceBuilderPtr CreateBuilder() override = 0;

        // IComponentSetup 인터페이스 (파생 클래스에서 구현)
        // DataRepositoryPtr CreateRepository() override = 0;
        // VisionEventHandlerPtr CreateVisionProcessor() override = 0;
        // void ConfigureParams(VisionContextPtr context) override = 0;
    };
}
