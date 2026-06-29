#pragma once

#include "Types.h"
#include <string>
#include <memory>

namespace VMF
{
    /// <summary>
    /// 시퀀스(Sequence) 생성 및 이름 반환을 위한 인터페이스.
    /// 컴포넌트(VisionProcessor, DataRepository) 생성은 IComponentSetup을 참조하세요.
    /// </summary>
    class ISequenceSetup
    {
    public:
        virtual ~ISequenceSetup() = default;

        /// 실행할 시퀀스 이름 반환
        virtual std::string GetSequenceName() const = 0;

        /// <summary>
        /// Equipment에 카메라별 Preset 파라미터 제공 (신규)
        /// </summary>
        virtual StringMap GetVisionParams(const std::string& presetName) const
        {
            return StringMap(); // 기본 구현 제공 — 필요한 Strategy에서만 override
        }

        /// 사용할 SequenceBuilder 생성
        virtual SequenceBuilderPtr CreateBuilder() = 0;
    };
}
