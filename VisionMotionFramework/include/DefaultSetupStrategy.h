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

        /// ConnectionManager 사용 여부 판단
        bool IsUsingConnectionManager() const;

        /// ConnectionManager로부터 공유 Controller 획득
        std::shared_ptr<VC::Controller> GetOrCreateSharedController();

        void ConfigureParams(VisionContextPtr context) override;
        DataRepositoryPtr CreateRepository() override;
        VisionProcessorPtr CreateVisionProcessor() override;

        // ISequenceSetup 순수 가상 함수 — 파생 클래스에서 반드시 구현
        virtual SequenceBuilderPtr CreateBuilder() override = 0;
        
        /// <summary>
        /// Context에 파라미터를 직접 설정합니다. (Builder 없이 직접 설정)
        /// </summary>
        /// <details>
        /// Builder 중간 단계 없이 Strategy가 직접 Context에 파라미터를 설정합니다.
        /// 4단계 리팩토링(Strategy → Context 직접 전달) 적용 시 사용됩니다.
        /// 기본 구현은 비어있으며, 파생 클래스에서 필요에 따라 오버라이드합니다.
        /// </details>
        /// <param name="ctx">파라미터를 설정할 Context</param>
        virtual void ConfigureContext(Context& ctx) override
        {
            (void)ctx;  // 기본 구현은 아무것도 하지 않음
        }
    };
}
