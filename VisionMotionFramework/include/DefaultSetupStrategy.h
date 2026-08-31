// D:\01GitHub_PROJECT\VisionMotionFramework\VisionMotionFramework\include\DefaultSetupStrategy.h
#pragma once

#include "VMF_API.h"
#include "IComponentSetup.h"
#include "ISequenceSetup.h"
#include "VisionConnectionManager.h"
#include "IActuator.h"
#include "VisionComm\Controller.h"
#include "IVisionClient.h"
#include "IDataRepository.h"
#include "SqliteDataRepository.h"
#include "Mock/CMockVisionClient.h"
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
    /// 
    /// @note 파라미터 관련 함수(GetDefaultParams, GetSetupParams 등)는
    ///       파생 클래스에서 오버라이드하여 플러그인별 파라미터를 제공합니다.
    ///       기본 구현은 빈 TaskParams를 반환합니다.
    /// </details>

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

        // ── 파라미터 관련 (파생 클래스에서 오버라이드) ──
        /// <summary>
        /// 모든 Task에 공통으로 적용될 기본 파라미터를 반환합니다.
        /// </summary>
        virtual TaskParams GetDefaultParams() const;

        /// <summary>
        /// 모든 Task에 공통으로 적용될 Vision 파라미터를 반환합니다.
        /// </summary>
        virtual TaskParams GetVisionParams() const;

        /// <summary>
        /// Context에 Task별 파라미터를 주입합니다.
        /// 기본 구현은 GetDefaultParams()를 호출하여 공통 파라미터를 설정합니다.
        /// 파생 클래스에서 Task별 파라미터를 추가하도록 오버라이드할 수 있습니다.
        /// </summary>
        virtual void SetTaskParamsByTask(Context& ctx) const;

        // ── IComponentSetup 인터페이스 ──
        /// <summary>
        /// preset 이름에 따라 Vision 파라미터를 제공합니다.
        /// 기본 구현은 GetVisionParams()를 호출하여 공통 Vision 파라미터를 반환합니다.
        /// </summary>
        StringMap GetVisionParams(const std::string& presetName) const override;

    private:
        bool IsUsingVisionConnectionManager() const;
        std::shared_ptr<VC::Controller> GetOrCreateSharedController();
    };
}
