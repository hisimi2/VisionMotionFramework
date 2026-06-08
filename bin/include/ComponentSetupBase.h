#pragma once

#include "VMF_API.h"
#include "IComponentSetup.h"
#include "Context.h"
#include "IActuator.h"
#include <sstream>

namespace VMF
{
    /// <summary>
    /// IComponentSetup의 기본 구현 클래스.
    /// SetParam, AddVisionPoint 등의 헬퍼 메서드를 제공합니다.
    /// CreateRepository, CreateVisionProcessor, ConfigureParams는 파생 클래스에서 구현해야 합니다.
    /// </summary>
    class VMF_API ComponentSetupBase : public IComponentSetup
    {
    protected:
        IActuator* m_adapter;

    public:
        ComponentSetupBase();
        ~ComponentSetupBase() override = default;

        void SetActuator(IActuator* adapter);
        IActuator* GetActuator();

        // IComponentSetup 인터페이스 (파생 클래스에서 구현)
        // DataRepositoryPtr CreateRepository() override = 0;
        // VisionEventHandlerPtr CreateVisionProcessor() override = 0;
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