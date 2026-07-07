#pragma once
#include "VMF_API.h"
#include "ISequenceSetup.h"
#include "Types.h"
#include <memory>

namespace VMF
{
    /// <summary>
    /// ISequenceSetup의 기본 구현 클래스.
    /// IComponentSetup과 분리되어 시퀀스 생성 책임만 담당합니다.
    /// 파생 클래스에서 BuildSequence, GetSequenceName, GetVisionParams 등을 오버라이드하여 사용합니다.
    /// </summary>
    class VMF_API SequenceFactoryBase : public ISequenceSetup
    {
    public:
        SequenceFactoryBase() = default;
        virtual ~SequenceFactoryBase() = default;

        // ISequenceSetup
        virtual std::string GetSequenceName() const override = 0;
        virtual SequenceBuilderPtr CreateBuilder() override = 0;
        virtual StringMap GetVisionParams(const std::string& preset) const override
        {
            return StringMap(); // 기본 구현: 빈 맵 반환
        }
    };
}