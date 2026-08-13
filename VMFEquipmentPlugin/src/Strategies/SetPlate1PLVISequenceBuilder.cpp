#include "pch.h"
#include "SetPlate1PLVISequenceBuilder.h"
#include "Context.h"

#include "src\Tasks\SetPlate1PLVISetup.h"
#include "src\Tasks\SetPlate1PLVIExecuteScan.h"
#include "src\Tasks\SetPlate1PLVIFinish.h"

using namespace VMF;
using namespace VMF_PLUGIN;

void SetPlate1PLVISequenceBuilder::ConfigureContext(VMF::Context& ctx)
{
    // Context에 시퀀스 실행에 필요한 파라미터를 설정
    // 이 파라미터들은 Tasks가 Context를 통해 접근할 수 있음
    
    // VisionParams 설정 (모든 Task에서 공통으로 사용하는 파라미터)
    VisionParams params;
    
    // 기본 파라미터 설정
    params.visionParams["HandID"] = "1";
    params.visionParams["PkgID"] = "1";
    params.visionParams["PLVI_POSITION"] = "0";
    params.visionParams["TIMEOUT_MEASURE_MS"] = "5000";
    params.visionParams["TIMEOUT_SCAN_MS"] = "15000";
    params.visionParams["TIMEOUT_RESULT_MS"] = "10000";
    params.visionParams["TIMEOUT_MOVE_MS"] = "7000";
    params.visionParams["SCAN_SPEED_MM_S"] = "100.0";
    params.visionParams["TRIGGER_INTERVAL_MM"] = "2.0";

    // Vision 파라미터
    params.visionParams["DATA_ID"] = "1";
    params.visionParams["PKG_NAME"] = "TEST_PKG";

    int ctrayX = 8, ctrayY = 4;
    params.visionParams["CTRAY_X"] = std::to_string(ctrayX);
    params.visionParams["CTRAY_Y"] = std::to_string(ctrayY);

    // Handler 포켓별 Device 정보 (기본값 Device 종류 = 99)
    const int totalPockets = ctrayX * ctrayY;
    for (int i = 0; i < totalPockets; ++i)
    {
        std::string key = "DEVICE_INFO_" + std::to_string(i);
        params.visionParams[key] = "99";
    }

    // ExecuteScan Task용 파라미터 설정
    params.visionParams["SCAN_END_Y"] = "200.0";

    // VisionPositions 설정 (시작 위치)
    VisionPosition startPos;
    startPos.pos = {0.0, 0.0, 0.0};  // X=0, Y=0, Z=0
    startPos.locateId = 0;
    startPos.visionRequestId = 1;
    params.visionPositions.push_back(startPos);
    
    // Context에 파라미터 설정
    ctx.SetTaskParams(params);
}

VMF::SequencePtr SetPlate1PLVISequenceBuilder::BuildSequence(const std::string& sequenceName)
{
    VMF::SequencePtr seq(new VMF::Sequence(sequenceName));

    // 1. Setup Task
    {
        auto task = std::make_shared<SetPlate1PLVISetup>();
        seq->AddTask(task);
    }

    // 2. Execute Scan Task
    {
        auto task = std::make_shared<SetPlate1PLVIExecuteScan>();
        seq->AddTask(task);
    }

    // 3. Finish Task
    {
        auto task = std::make_shared<SetPlate1PLVIFinish>();
        seq->AddTask(task);
    }

    return seq;
}
