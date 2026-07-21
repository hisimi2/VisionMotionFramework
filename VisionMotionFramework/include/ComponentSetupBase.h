#pragma once

#include "VMF_API.h"
#include "IComponentSetup.h"
#include "ConnectionManager.h"
#include "IActuator.h"
#include "Controller.h"
#include "IVisionClient.h"
#include "IDataRepository.h"
#include <memory>

namespace VMF
{
    /// <summary>
    /// IComponentSetup의 기본 구현 클래스.
    /// 
    /// [책임 범위]
    /// - IComponentSetup 인터페이스 구현 (Repository, VisionProcessor 생성)
    /// - Connection 관리 (SetConnectionConfig, GetOrCreateSharedController)
    /// - Actuator 설정 (SetActuator, GetActuator)
    /// - CreateRepository(), CreateVisionProcessor() 기본 구현 제공
    ///   (SqliteDataRepository + CMockVisionEventHandler)
    ///   → 파생 클래스에서 필요 시 오버라이드
    /// 
    /// ※ 시퀀스 생성(GetSequenceName, CreateBuilder)은 SequenceFactoryBase(ISequenceSetup)로 분리되었습니다.
    ///   두 인터페이스가 모두 필요한 경우, 다중 상속 또는 컴포지션 패턴을 사용하세요.
    /// </summary>
    class VMF_API ComponentSetupBase : public virtual IComponentSetup
    {
    private:
        IActuator* m_adapter;
        VisionConnectionConfig m_connectionConfig;

    public:
        ComponentSetupBase();
        ~ComponentSetupBase() override = default;

        // IComponentSetup
        void SetActuator(IActuator* adapter) override;
        IActuator* GetActuator() override;

        void SetConnectionConfig(const VisionConnectionConfig& config) override;
        const VisionConnectionConfig& GetConnectionConfig() const override;

        /// ConnectionManager 사용 여부 판단
        bool IsUsingConnectionManager() const;

        /// ConnectionManager로부터 공유 Controller 획득
        std::shared_ptr<VC::Controller> GetOrCreateSharedController();

        void ConfigureParams(VisionContextPtr context) override;
        DataRepositoryPtr CreateRepository() override;
        VisionProcessorPtr CreateVisionProcessor() override;
    };
}
