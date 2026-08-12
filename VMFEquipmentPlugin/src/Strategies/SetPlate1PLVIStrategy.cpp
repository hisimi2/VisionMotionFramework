#include "pch.h"
#include "SetPlate1PLVIStrategy.h"
#include "SqliteDataRepository.h"
#include "..\Protocol\VisionPlviProcessor.h"
#include "..\Strategies\SetPlate1PLVISequenceBuilder.h"
#include <sstream>
#include <vector>

using namespace VMF;
using namespace VMF_PLUGIN;

std::string SetPlate1PLVIStrategy::GetSequenceName() const
{
    return "SetPlate1PLVI";
}

VMF::DataRepositoryPtr SetPlate1PLVIStrategy::CreateRepository()
{
    auto repo = std::make_shared<VMF::SqliteDataRepository>(
        "Data\\PLVI_DATABASE.db", "Data\\Images");
    repo->Initialize();
    return repo;
}

VMF::VisionProcessorPtr SetPlate1PLVIStrategy::CreateVisionProcessor()
{
    VMF::VisionConnectionConfig config("127.0.0.1", 8000, 3000);
    auto vm = std::make_shared<VisionPlviProcessor>();
    vm->Initialize(config);
    return vm;
}

VMF::SequenceBuilderPtr SetPlate1PLVIStrategy::CreateBuilder()
{
    return std::make_shared<VMF_PLUGIN::SetPlate1PLVISequenceBuilder>();
}

void SetPlate1PLVIStrategy::ConfigureParams(VMF::VisionContextPtr ctx)
{
    auto repo = ctx->GetRepository();
    VMF::VisionParams params;

    // 기본 파라미터 설정
    auto setParam = [&params](const std::string& key, const std::string& value) {
        params.visionParams[key] = value;
    };
    auto setParamInt = [&params](const std::string& key, int value) {
        params.visionParams[key] = std::to_string(value);
    };
    auto setParamDouble = [&params](const std::string& key, double value) {
        std::ostringstream oss;
        oss << value;
        params.visionParams[key] = oss.str();
    };
    auto addVisionPoint = [&params](int locateId, int requestId, double x, double y, double z) {
        std::vector<double> pos = {x, y, z};
        params.visionPositions.push_back(VMF::VisionPosition(pos, locateId, requestId));
    };
    auto saveVisionParamsToRepo = [&](VMF::VisionContextPtr ctx, const VMF::VisionParams& params) {
        auto repo = ctx->GetRepository();
        if (!repo) return;
        for (const auto& kv : params.visionParams) {
            repo->SaveParam("PLVI", kv.first, kv.second);
        }
    };

    setParamInt("HandID", 1);
    setParamInt("PkgID", 1);
    setParamInt("PLVI_POSITION", 0);
    setParamInt("TIMEOUT_MEASURE_MS", 5000);
    setParamInt("TIMEOUT_SCAN_MS", 15000);
    setParamInt("TIMEOUT_RESULT_MS", 10000);
    setParamInt("TIMEOUT_MOVE_MS", 7000);
    setParamDouble("SCAN_SPEED_MM_S", 100.0);
    setParamDouble("TRIGGER_INTERVAL_MM", 2.0);

    // Vision 파라미터
    setParamInt("DATA_ID", 1);
    setParam("PKG_NAME", "TEST_PKG");

    int ctrayX = 8, ctrayY = 4;
    setParamInt("CTRAY_X", ctrayX);
    setParamInt("CTRAY_Y", ctrayY);

    // Handler 포켓별 Device 정보 (기본값 Device 종류 = 99)
    const int totalPockets = ctrayX * ctrayY;
    for (int i = 0; i < totalPockets; ++i)
    {
        std::string key = "DEVICE_INFO_" + std::to_string(i);
        setParamInt(key, 99);
    }

    // visionPositions - LoadResult(InspInitPos) 사용
    const int nLocateId = 0;
    const int nVisionRequestId = 1;
    const int pkgId = 1;

    double scanStartX = 0.0, scanStartY = 0.0, scanStartZ = 0.0;

    /*if (repo)
    {
        VMF::StringMap q;
        q[CAM_INDEX] = "0";    // PLVI 카메라 인덱스
        q[LOCATION_ID] = std::to_string(nLocateId);
        q[PKG_ID] = std::to_string(pkgId);

        VMF::StringMap out;
        if (repo->LoadResult(InspInitPos, q, out) == StorageSuccess)
        {
            scanStartX = std::atof(out[POS_X].c_str());
            scanStartY = std::atof(out[POS_Y].c_str());
            scanStartZ = std::atof(out[FOCUS].c_str());
        }
    }*/

    addVisionPoint(nLocateId, nVisionRequestId, scanStartX, scanStartY, scanStartZ);

    // Repository에 params 저장
    saveVisionParamsToRepo(ctx, params);
}

VMF::StringMap SetPlate1PLVIStrategy::GetVisionParams(const std::string& presetName) const
{
    VMF::StringMap p;
    return VMF::StringMap();
}


