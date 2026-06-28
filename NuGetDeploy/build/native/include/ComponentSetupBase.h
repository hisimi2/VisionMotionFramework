#pragma once

#include "VMF_API.h"
#include "IComponentSetup.h"
#include "ISequenceSetup.h"
#include "Context.h"
#include "IActuator.h"
#include "ConnectionManager.h"
#include "Controller.h"
#include <sstream>

namespace VMF
{
    /// <summary>
    /// IComponentSetup의 기본 구현 클래스.
    /// SetParam, AddVisionPoint 등의 헬퍼 메서드를 제공합니다.
    /// CreateRepository, CreateVisionProcessor, ConfigureParams는 파생 클래스에서 구현해야 합니다.
    /// 
    /// 다중 서버 대응:
    /// SetConnectionConfig()로 Vision 서버 연결 설정을 주입하면,
    /// GetOrCreateSharedController()를 통해 ConnectionManager가 관리하는
    /// 공유 Controller를 획득할 수 있습니다.
    /// 동일 서버(IP:Port)는 단일 소켓, 다른 서버는 별도 소켓을 자동으로 관리합니다.
    /// </summary>
    class VMF_API ComponentSetupBase : public IComponentSetup, public ISequenceSetup
    {
    protected:
        IActuator* m_adapter;
        /// <summary>
        /// Vision 서버 연결 설정 (선택적).
        /// 설정된 경우 GetOrCreateSharedController()가 ConnectionManager를
        /// 통해 공유 Controller를 반환합니다.
        /// </summary>
        VisionConnectionConfig m_connectionConfig;
        /// <summary>
        /// m_connectionConfig가 설정되었는지 여부
        /// </summary>
        bool m_useConnectionManager;

    public:
        ComponentSetupBase();
        ~ComponentSetupBase() override = default;

        void SetActuator(IActuator* adapter);
        IActuator* GetActuator();

        /// <summary>
        /// Vision 서버 연결 설정을 주입합니다.
        /// 이 값을 설정하면 GetOrCreateSharedController()를 통해
        /// ConnectionManager가 관리하는 공유 Controller를 획득할 수 있습니다.
        /// </summary>
        void SetConnectionConfig(const VisionConnectionConfig& config);
        
        /// <summary>
        /// 현재 설정된 연결 정보를 반환합니다.
        /// </summary>
        const VisionConnectionConfig& GetConnectionConfig() const;

        /// <summary>
        /// ConnectionManager 사용 여부
        /// </summary>
        bool IsUsingConnectionManager() const;

        /// <summary>
        /// ConnectionManager로부터 공유 Controller를 획득합니다.
        /// 동일 IP:Port에 대해 이미 연결된 소켓이 있으면 기존 Controller 반환,
        /// 없으면 새로 생성하여 반환합니다.
        /// 
        /// 파생 클래스의 CreateVisionProcessor()에서 이 Controller를 받아
        /// VisionProcessor::InitializeWithSharedController()를 호출하여 사용합니다.
        /// </summary>
        std::shared_ptr<VC::Controller> GetOrCreateSharedController();

        // IComponentSetup 인터페이스 (파생 클래스에서 구현)
        // DataRepositoryPtr CreateRepository() override = 0;
        // VisionProcessorPtr CreateVisionProcessor() override = 0;
        // void ConfigureParams(VisionContextPtr context) override = 0;

    protected:
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
