#pragma once
#include "DefaultSetupStrategy.h"
#include "SqliteDataRepository.h"
#include "..\VisionMotionFramework\include\Mock\CMockVisionEventHandler.h"
#include <memory>
#include <sstream>


#include "VMFEquipmentPluginExport.h"

/// <summary>
/// 메모리 기반 시퀀스 전략을 위한 기본 클래스입니다.
/// Task별 파라미터는 Builder의 SetTaskParams()로 직접 주입합니다.
/// </summary>
class VMF_PLUGIN_API CLoad1VATBaseSequenceStrategy : public VMF::DefaultSetupStrategy
{
public:
    virtual void ConfigureParams(std::shared_ptr<VMF::Context> ctx) = 0;  // = 0 추가
    VMF::DataRepositoryPtr CreateRepository() override
    {
        auto repo = std::make_shared<VMF::SqliteDataRepository>("Data\\VAT_DATABASE.db", "Data\\Images");
        repo->Initialize();

        return repo;
    }

    VMF::VisionProcessorPtr CreateVisionProcessor() override
    {
        VMF::VisionConnectionConfig config("127.0.0.1", 8080, 3000);
        auto vm = std::make_shared<VMF::CMockVisionEventHandler>();
        vm->Initialize(config);

        return vm;
    }

protected:
    /// <summary>
    /// Helper: VisionParams에 문자열 파라미터 설정
    /// </summary>
    void SetParam(VMF::VisionParams& params, const std::string& key, const std::string& value)
    {
        params.visionParams[key] = value;
    }

    /// <summary>
    /// Helper: VisionParams에 정수 파라미터 설정 (자동 문자열 변환)
    /// </summary>
    void SetParam(VMF::VisionParams& params, const std::string& key, int value)
    {
        params.visionParams[key] = std::to_string(value);
    }

    /// <summary>
    /// Helper: VisionParams에 실수 파라미터 설정 (자동 문자열 변환)
    /// </summary>
    void SetParam(VMF::VisionParams& params, const std::string& key, double value)
    {
        std::ostringstream oss;
        oss << value;
        params.visionParams[key] = oss.str();
    }

    /// <summary>
    /// Helper: VisionPosition 추가 (3축)
    /// </summary>
    void AddVisionPoint(VMF::VisionParams& params, int locateId, int requestId,
        double x, double y, double z)
    {
        std::vector<double> pos;
        pos.push_back(x);
        pos.push_back(y);
        pos.push_back(z);
        params.visionPositions.push_back(VMF::VisionPosition(pos, locateId, requestId));
    }

    /// <summary>
    /// Helper: VisionPosition 추가 (5축)
    /// </summary>
    void AddVisionPoint(VMF::VisionParams& params, int locateId, int requestId,
        double x, double y, double z, double t1, double t2)
    {
        std::vector<double> pos;
        pos.push_back(x);
        pos.push_back(y);
        pos.push_back(z);
        pos.push_back(t1);
        pos.push_back(t2);
        params.visionPositions.push_back(VMF::VisionPosition(pos, locateId, requestId));
    }
};





