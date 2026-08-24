#pragma once

#include "VMF_API.h"
#include "Types.h"
#include "IActuator.h"
#include "IVisionClient.h"
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
        virtual VisionProcessorPtr CreateVisionProcessor() = 0;

        /// <summary>
        /// 생성된 Context에 파라미터를 설정합니다.
        /// </summary>
        virtual void ConfigureParams(VisionContextPtr context) = 0;

        // ---- Actuator 설정 (IComponentSetup 레벨로 승격) ----
        /// <summary>
        /// Actuator(IActuator*)를 설정합니다.
        /// </summary>
        virtual void SetActuator(IActuator* adapter) = 0;

        /// <summary>
        /// 설정된 Actuator를 반환합니다.
        /// </summary>
        virtual IActuator* GetActuator() = 0;

        // ---- Connection 설정 ----
        /// <summary>
        /// Vision 서버 연결 설정을 저장합니다.
        /// </summary>
        virtual void SetConnectionConfig(const VisionConnectionConfig& config) = 0;

        /// <summary>
        /// 저장된 연결 설정을 반환합니다.
        /// </summary>
        virtual const VisionConnectionConfig& GetConnectionConfig() const = 0;

        /// <summary>
       /// Equipment에 카메라별 Preset 파라미터 제공 (신규)
       /// </summary>
        virtual StringMap GetVisionParams(const std::string& presetName) const = 0;
    };
}
