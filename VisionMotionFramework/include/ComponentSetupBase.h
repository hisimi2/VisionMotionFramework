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
    /// SetParam, AddVisionPoint 등의 헬퍼 메서드를 제공합니다.
    /// CreateRepository, CreateVisionProcessor, ConfigureParams는 파생 클래스에서 구현해야 합니다.
    /// 
    /// [Task params 관리]
    /// Strategy(파생 클래스)는 ConfigureParams()에서 Task별 VisionParams를 생성하고,
    /// 직접 Builder로 전달하거나 Context를 통해 Task에 주입합니다.
    /// Builder는 BuildSequence()에서 Task 생성 후 SetTaskParams()를 호출합니다.
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

        /// 문자열 파라미터 설정
        void SetParam(VisionParams& params, const std::string& key, const std::string& value);

        /// 정수 파라미터 설정 (자동 문자열 변환)
        void SetParam(VisionParams& params, const std::string& key, double value);

        /// 비전 검사 위치 추가 (3축)
        void AddVisionPoint(VisionParams& params, int locateId, int requestId, double x, double y, double z);

/// 비전 검사 위치 추가 (5축)
        void AddVisionPoint(VisionParams& params, int locateId, int requestId, double x, double y, double z, double t1, double t2);
    };
}
