#pragma once

#include "VMF_API.h"
#include "IComponentSetup.h"
#include "ISequenceSetup.h"
#include "ConnectionManager.h"
#include "IActuator.h"
#include "Controller.h"
#include "IVisionClient.h"
#include "IDataRepository.h"
#include "SqliteDataRepository.h"
#include "Mock/CMockVisionEventHandler.h"
#include <string>
#include <memory>

namespace VMF
{
    /// <summary>
    /// 기본 설정 전략 (Default Setup Strategy)
    /// </summary>
    /// <details>
    /// IComponentSetup과 ISequenceSetup을 상속받는 기본 전략 클래스입니다.
    /// 파생 클래스에서 필요한 메서드를 오버라이드하여 사용합니다.
    /// ComponentSetupBase의 기능을 직접 구현하여 중간 클래스를 제거했습니다.
    
    class VMF_API DefaultSetupStrategy : public IComponentSetup, public ISequenceSetup
    {
    private:
        IActuator* m_adapter;
        VisionConnectionConfig m_connectionConfig;

    public:
        DefaultSetupStrategy();
        virtual ~DefaultSetupStrategy() = default;

        // IComponentSetup
        void SetActuator(IActuator* adapter) override;
        IActuator* GetActuator() override;
        void SetConnectionConfig(const VisionConnectionConfig& config) override;
        const VisionConnectionConfig& GetConnectionConfig() const override;
        void ConfigureParams(VisionContextPtr context) override;
        DataRepositoryPtr CreateRepository() override;
        VisionProcessorPtr CreateVision() override;

        // ISequenceSetup
        virtual SequenceBuilderPtr CreateBuilder() override = 0;

    private:
        bool IsUsingConnectionManager() const;
        std::shared_ptr<VC::Controller> GetOrCreateSharedController();
    };
}
