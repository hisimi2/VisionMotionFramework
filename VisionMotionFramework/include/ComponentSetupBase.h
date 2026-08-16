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
