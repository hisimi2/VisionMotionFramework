#pragma once

#include "VMF_API.h"
#include "IComponentSetup.h"
#include "ISequenceSetup.h"
#include "ConnectionManager.h"
#include "IActuator.h"
#include "Controller.h"
#include "IVisionProcessor.h"
#include "IDataRepository.h"
#include <memory>

namespace VMF
{
/// <summary>
    /// IComponentSetup의 기본 구현 클래스.
    /// 
    /// [책임 범위]
    /// - IComponentSetup + ISequenceSetup 인터페이스 통합
    /// - Connection 관리 (SetConnectionConfig, GetOrCreateSharedController)
    /// - Actuator 설정 (SetActuator, GetActuator)
    /// - CreateRepository(), CreateVisionProcessor() 기본 구현 제공
    ///   (SqliteDataRepository + CMockVisionEventHandler)
    ///   → 파생 클래스에서 필요 시 오버라이드
    /// 
    /// ConfigureParams는 기본 구현(empty)을 제공하며, 필요 시 오버라이드하세요.
    /// VisionParams 헬퍼(SetParam, AddVisionPoint)는 SequenceBuilderBase에서 제공합니다.
    /// </summary>
    class VMF_API ComponentSetupBase : public IComponentSetup, public ISequenceSetup
    {
    private:
        IActuator* m_adapter;
        VisionConnectionConfig m_connectionConfig;

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

        /// <summary>
        /// ConfigureParams 기본 구현 (empty).
        /// 파생 클래스에서 필요 시 오버라이드하세요.
        /// </summary>
        void ConfigureParams(VisionContextPtr context) override;

        /// <summary>
        /// SqliteDataRepository를 기본으로 생성합니다.
        /// 파생 클래스에서 오버라이드하여 다른 저장소를 사용할 수 있습니다.
        /// </summary>
        DataRepositoryPtr CreateRepository() override;

        /// <summary>
        /// CMockVisionEventHandler를 기본으로 생성합니다.
        /// ConnectionManager 모드가 설정된 경우 공유 Controller를 사용합니다.
        /// 파생 클래스에서 오버라이드하여 다른 프로세서를 사용할 수 있습니다.
        /// </summary>
        VisionProcessorPtr CreateVisionProcessor() override;
    };
}
