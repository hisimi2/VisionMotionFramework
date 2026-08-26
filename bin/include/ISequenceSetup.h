#pragma once

#include "Types.h"
#include "Context.h"
#include <string>
#include <memory>

namespace VMF
{

    /// <summary>
    /// 시퀀스(Sequence) 생성 및 이름 반환을 위한 인터페이스.
    /// 컴포넌트(VisionProcessor, DataRepository) 생성은 IComponentSetup을 참조하세요.
    /// </summary>
    class VMF_API ISequenceSetup
    {
    public:
        virtual ~ISequenceSetup() = default;

        /// 실행할 시퀀스 이름 반환
        virtual std::string GetName() const = 0;

        /// 사용할 SequenceBuilder 생성
        virtual SequenceBuilderPtr CreateBuilder() = 0;

        /// <summary>
        /// Context에 파라미터를 직접 설정합니다. (Builder 없이 직접 설정)
        /// </summary>
        /// <details>
        /// Builder 중간 단계 없이 Strategy가 직접 Context에 파라미터를 설정합니다.
        /// 4단계 리팩토링(Strategy → Context 직접 전달) 적용 시 사용됩니다.
        /// 기본 구현은 비어있으며, 파생 클래스에서 필요에 따라 오버라이드합니다.
        /// </details>
        /// <param name="ctx">파라미터를 설정할 Context</param>
        virtual void ConfigureContext(Context& ctx)
        {
            (void)ctx;  // 기본 구현은 아무것도 하지 않음
        }
    };
}
