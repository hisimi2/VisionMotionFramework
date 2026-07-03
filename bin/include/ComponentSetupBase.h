#pragma once

#include "VMF_API.h"
#include "IComponentSetup.h"
#include "ISequenceSetup.h"
#include "ConnectionManager.h"
#include "Context.h"
#include "IActuator.h"
#include "Controller.h"
#include <sstream>
#include <memory>

namespace VMF
{
    /// <summary>
    /// IComponentSetup의 기본 구현 클래스.
    /// SetParam, AddVisionPoint 등의 헬퍼 메서드는 SequenceBuilderBase로 이동되었습니다.
    /// CreateRepository, CreateVisionProcessor, ConfigureParams는 파생 클래스에서 구현해야 합니다.
    /// 
    /// [책임 범위]
    /// - IComponentSetup + ISequenceSetup 인터페이스 통합
    /// - Connection 관리 (SetConnectionConfig, GetOrCreateSharedController)
    /// - Actuator 설정 (SetActuator, GetActuator)
    /// </summary>
    class VMF_API ComponentSetupBase : public IComponentSetup, public ISequenceSetup
    {
    protected:
        IActuator* m_adapter;

    public:
        ComponentSetupBase();
        ~ComponentSetupBase() override = default;

        void SetActuator(IActuator* adapter);
        IActuator* GetActuator();

        /// 연결 설정 저장
        void SetConnectionConfig(const VisionConnectionConfig& config);
        const VisionConnectionConfig& GetConnectionConfig() const;

        /// ConnectionManager 사용 여부 판단
        bool IsUsingConnectionManager() const;

        /// ConnectionManager로부터 공유 Controller 획득
        std::shared_ptr<VC::Controller> GetOrCreateSharedController();

        // IComponentSetup 인터페이스 (파생 클래스에서 구현)
        // DataRepositoryPtr CreateRepository() override = 0;
        // VisionProcessorPtr CreateVisionProcessor() override = 0;
        // void ConfigureParams(VisionContextPtr context) override = 0;

    protected:
        VisionConnectionConfig m_connectionConfig;
    };
}
