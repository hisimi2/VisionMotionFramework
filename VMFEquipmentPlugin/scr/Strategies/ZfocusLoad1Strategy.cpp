#include "pch.h"
#include "ZfocusLoad1Strategy.h"
#include "ZFocusLoad1Builder.h"
#include "SqliteDataRepository.h"
#include "scr\protocol\VisionProcessor.h"

#include "Mock\CMockVisionProcessor.h"
#include "Mock\CMockDataRepository.h"


namespace VMF_Sample
{
    DataRepositoryPtr ZfocusLoad1Strategy::CreateRepository()
    {
        // SqliteDataRepository 생성 및 초기화
        auto repo = std::make_shared<VMF::SqliteDataRepository>(
            "Data\\VAT_DATABASE.db", "Data\\Images");
        repo->Initialize();

        return repo;
    }

    VisionProcessorPtr ZfocusLoad1Strategy::CreateVisionProcessor()
    {
        // ConnectionManager 모드 확인
        if (IsUsingConnectionManager())
        {
            auto sharedCtrl = GetOrCreateSharedController();
            if (sharedCtrl)
            {
                auto vp = std::make_shared<VMF::VisionProcessor>();
                VC::Status status = vp->InitializeWithSharedController(
                    sharedCtrl, GetConnectionConfig());
                if (status == VC::VisionOK)
                {
                    return vp;
                }
            }
        }

        // 기본 모드: VisionProcessor 생성 및 초기화
        // 생성자에서 주입된 ConnectionConfig 우선 사용, 없으면 기본값
        VMF::VisionConnectionConfig config = GetConnectionConfig();
        if (config.address.empty() || config.port == 0)
        {
            config = VMF::VisionConnectionConfig("127.0.0.1", 8080, 3000);
        }

        auto vp = std::make_shared<VMF::CMockVisionProcessor>();
        // auto vp = std::make_shared<VMF::VisionProcessor>();
        vp->Initialize(config);
        return vp;
    }

    std::string ZfocusLoad1Strategy::GetSequenceName() const
    {
        return "SampleZFocus";
    }

    SequenceBuilderPtr ZfocusLoad1Strategy::CreateBuilder()
    {
        return std::make_shared<ZFocusLoad1Builder>();
    }

    void ZfocusLoad1Strategy::ConfigureParams(VMF::VisionContextPtr context) 
    {
        if (!context)
            return;

        // Example: Set default camera index, package ID, etc.
        // context->SetSeqParam("CameraIndex", "6");
        // context->SetSeqParam("PkgID", "1");
        // context->SetSeqParam("TimeOutMs", "10000");

        // !!! Modification Required: Add equipment-specific preset parameters here !!!
    }
}

