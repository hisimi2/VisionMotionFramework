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
#include <unordered_map>

namespace VMF
{
    /// <summary>
    /// IComponentSetup의 기본 구현 클래스.
    /// SetParam, AddVisionPoint 등의 헬퍼 메서드를 제공합니다.
    /// CreateRepository, CreateVisionProcessor, ConfigureParams는 파생 클래스에서 구현해야 합니다.
    /// 
    /// [Task params 관리]
    /// Strategy(파생 클래스)는 ConfigureParams()에서 SetTaskParamForTask()를 호출하여
    /// Task별 VisionParams를 설정합니다. 이 params는 Builder로 전달되어
    /// Task 생성 시 SetTaskParams()로 주입됩니다.
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

        /// <summary>
        /// Task별 VisionParams를 설정합니다.
        /// ConfigureParams()에서 호출하여 Builder로 전달할 Task params를 저장합니다.
        /// </summary>
        /// <param name="taskName">Task 이름 (GetName() 반환값과 일치)</param>
        /// <param name="params">Task 전용 VisionParams</param>
        void SetTaskParamForTask(const std::string& taskName, const VisionParams& params);

        /// <summary>
        /// 저장된 Task별 params 맵을 반환합니다.
        /// Orchestrator가 Builder로 전달할 때 사용됩니다.
        /// </summary>
        const std::unordered_map<std::string, VisionParams>& GetTaskParamsMap() const;

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

        /// Task별 params 저장소 (Strategy → Builder 전달)
        std::unordered_map<std::string, VisionParams> m_strategyTaskParams;
    };
}
