#pragma once
#include "ComponentSetupBase.h"
#include "SqliteDataRepository.h"
#include "..\VMFEquipmentPlugin\Protocol\VisionMemoryProcessor.h"
#include "..\VisionMotionFramework\include\Mock\CMockVisionEventHandler.h"
#include <memory>

/// <summary>
/// 메모리 기반 시퀀스 전략을 위한 기본 클래스입니다.
/// </summary>
class CLoad1VATBaseSequenceStrategy : public VMF::ComponentSetupBase
{
public:
    VMF::DataRepositoryPtr CreateRepository() override
    {
        // auto repo = std::make_shared<VMF::CMockDataRepository>();
        auto repo = std::make_shared<VMF::SqliteDataRepository>("Data\\VAT_DATABASE.db", "Data\\Images");
        repo->Initialize();

        return repo;
    }

    VMF::VisionProcessorPtr CreateVisionProcessor() override
    {
        VMF::VisionConnectionConfig config("127.0.0.1", 8080, 3000);
        auto vm = std::make_shared<VMF::CMockVisionEventHandler>();
        // auto vm = std::make_shared<VMF::VisionMemoryProcessor>();
        vm->Initialize(config);

        return vm;
    }

protected:
    /// <summary>
    /// SequenceStrategyBase.h의 ConfigureParams 구현에서 사용하여, 시퀀스 실행에 필요한 파라미터를 간편하게 설정할 수 있도록 확장합니다.
    /// </summary>
    // 문자열 파라미터 설정
    void SetParam(VMF::VisionParams& params, const std::string& key, const std::string& value)
    {
        params.seqParams[key] = value;
    }
    // 정수 파라미터 설정 (자동 문자열 변환)
    void SetParam(VMF::VisionParams& params, const std::string& key, double value)
    {
        std::ostringstream oss;
        oss << value;
        params.seqParams[key] = oss.str();
    }
    // 비전 검사 위치 추가
    void AddVisionPoint(VMF::VisionParams& params, int locateId, int requestId, double x, double y, double z)
    {
        std::vector<double> pos;
        pos.push_back(x);
        pos.push_back(y);
        pos.push_back(z);
        params.visionPositions.push_back(VMF::VisionPosition(pos, locateId, requestId));
    }
    // 비전 검사 위치 추가
    void AddVisionPoint(VMF::VisionParams& params, int locateId, int requestId, double x, double y, double z, double t1, double t2)
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





