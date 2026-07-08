#include "pch.h"
#include "SampleSequenceStrategy.h"
#include "SampleZFocusSequenceBuilder.h"
#include "SqliteDataRepository.h"
#include "VisionProcessor.h"

#include "Mock\CMockVisionProcessor.h"
#include "Mock\CMockDataRepository.h"


namespace VMF_Sample
{
    DataRepositoryPtr SampleSequenceStrategy::CreateRepository()
    {
        // SqliteDataRepository 생성 및 초기화
        auto repo = std::make_shared<VMF::SqliteDataRepository>(
            "Data\\VAT_DATABASE.db", "Data\\Images");
        repo->Initialize();

        return repo;
    }

    VisionProcessorPtr SampleSequenceStrategy::CreateVisionProcessor()
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
        VMF::VisionConnectionConfig config("127.0.0.1", 8080, 3000);

        auto vp = std::make_shared<VMF::CMockVisionProcessor>();
        // auto vp = std::make_shared<VMF::VisionProcessor>();
        vp->Initialize(config);
        return vp;
    }

    std::string SampleSequenceStrategy::GetSequenceName() const
    {
        return "SampleZFocus";
    }

    SequenceBuilderPtr SampleSequenceStrategy::CreateBuilder()
    {
        return std::make_shared<SampleZFocusSequenceBuilder>();
    }

    void SampleSequenceStrategy::ConfigureParams(VMF::VisionContextPtr context) 
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

