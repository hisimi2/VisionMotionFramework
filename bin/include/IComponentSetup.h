#pragma once

#include "VMF_API.h"
#include "Types.h"
#include <memory>

namespace VMF
{
    class Context;

    /// <summary>
    /// 컴포넌트(VisionProcessor, DataRepository) 생성 및 파라미터 설정을 위한 인터페이스.
    /// 상태머신 모드와 직접 모드가 공통으로 사용합니다.
    /// </summary>
    class VMF_API IComponentSetup
    {
    public:
        virtual ~IComponentSetup() = default;

        /// <summary>
        /// DataRepository 인스턴스를 생성합니다.
        /// </summary>
        virtual DataRepositoryPtr CreateRepository() = 0;

        /// <summary>
        /// VisionEventHandler 인스턴스를 생성합니다.
        /// </summary>
        virtual VisionEventHandlerPtr CreateVisionProcessor() = 0;

        /// <summary>
        /// 생성된 Context에 파라미터를 설정합니다.
        /// </summary>
        virtual void ConfigureParams(VisionContextPtr context) = 0;
    };
}