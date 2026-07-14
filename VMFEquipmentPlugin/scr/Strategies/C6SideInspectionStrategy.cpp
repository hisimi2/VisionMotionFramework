#include "pch.h"
#include "C6SideInspectionStrategy.h"
#include "scr\Protocol\Vision6SideProcessor.h"
#include "C6SideInspectionSequenceBuilder.h"

namespace VMF_PLUGIN
{
    std::string C6SideInspectionStrategy::GetSequenceName() const 
    {
        return "6SideInspection";
    }

    VMF::DataRepositoryPtr C6SideInspectionStrategy::CreateRepository() 
    {
        auto repo = std::make_shared<VMF::SqliteDataRepository>(
            "Data\\6SIDE_DATABASE.db", "Data\\Images");
        repo->Initialize();
        return repo;
    }

    VMF::VisionProcessorPtr C6SideInspectionStrategy::CreateVisionProcessor() 
    {
        VMF::VisionConnectionConfig config("127.0.0.1", 8001, 3000);
        auto vm = std::make_shared<VMF::Vision6SideProcessor>();
        vm->Initialize(config);
        return vm;
    }

    VMF::SequenceBuilderPtr C6SideInspectionStrategy::CreateBuilder() 
    {
        return std::make_shared<
            C6SideInspectionSequenceBuilder>();
    }

    void C6SideInspectionStrategy::ConfigureParams(VMF::VisionContextPtr ctx)
    {
        // BarcodeID, LotID는 실제 장비에서 DataManager에서 꺼내야 하나
        // 샘플에서는 더미값 사용
        const std::string barcodeId = "SN0000001";
        const std::string lotId = "TESTLOT_20240325_1209";

        // ── 공통 파라미터 ──────────────────────────────────────
        RegisterCommonParam()
            .Set("SelectCount", 6)      // 6면 전체 검사
            .Set("CameraIndex", 0)      // CamIndex #1
            .Set("VisionType", 0)      // Vision PC1
            .Set("GripperSafetyPos", 0.0)    // Gripper Safety 위치
            .End();

        // ── 면 1: Left ────────────────────────────────────────
        // Turn180_Left → CameraZ_Left → 촬영 (실린더 불필요)
        RegisterTagParam("Face_Left")
            .Set("FacePosition", 1)
            .Set("Turn180Pos", 10.0)    // Turn180 Left 위치 (레시피 값)
            .Set("Turn360Pos", 0.0)     // Turn360 불필요
            .Set("NeedCylOp", 0)
            .Set("CameraZPos", 5.0)     // CameraZ Left 위치
            .Set("BarcodeID", barcodeId)
            .Set("LotID", lotId)
            .Set("SelectCount", 6)
            .End();

        // ── 면 2: Right ───────────────────────────────────────
        // Turn180_Right → CameraZ_Right → 촬영
        RegisterTagParam("Face_Right")
            .Set("FacePosition", 2)
            .Set("Turn180Pos", -10.0)   // Turn180 Right 위치
            .Set("Turn360Pos", 0.0)
            .Set("NeedCylOp", 0)
            .Set("CameraZPos", 5.0)
            .Set("BarcodeID", barcodeId)
            .Set("LotID", lotId)
            .Set("SelectCount", 6)
            .End();

        // ── 면 3: Front ───────────────────────────────────────
        // Turn180_Safety → 실린더 Grip → Gripper Safety
        // → 실린더 Ungrip → 실린더 Back → Turn360_Front → 촬영
        RegisterTagParam("Face_Front")
            .Set("FacePosition", 3)
            .Set("Turn180Pos", 0.0)     // Turn180 Safety 위치
            .Set("Turn360Pos", 90.0)    // Turn360 Front 위치
            .Set("NeedCylOp", 1)       // 실린더 전환 필요
            .Set("CameraZPos", 10.0)
            .Set("BarcodeID", barcodeId)
            .Set("LotID", lotId)
            .Set("SelectCount", 6)
            .End();

        // ── 면 4: Top ─────────────────────────────────────────
        // Turn360_Top → 촬영
        RegisterTagParam("Face_Top")
            .Set("FacePosition", 4)
            .Set("Turn180Pos", 0.0)
            .Set("Turn360Pos", 180.0)   // Turn360 Top 위치
            .Set("NeedCylOp", 0)
            .Set("CameraZPos", 15.0)
            .Set("BarcodeID", barcodeId)
            .Set("LotID", lotId)
            .Set("SelectCount", 6)
            .End();

        // ── 면 5: Rear ────────────────────────────────────────
        // Turn360_Rear → 촬영
        RegisterTagParam("Face_Rear")
            .Set("FacePosition", 5)
            .Set("Turn180Pos", 0.0)
            .Set("Turn360Pos", 270.0)   // Turn360 Rear 위치
            .Set("NeedCylOp", 0)
            .Set("CameraZPos", 10.0)
            .Set("BarcodeID", barcodeId)
            .Set("LotID", lotId)
            .Set("SelectCount", 6)
            .End();

        // ── 면 6: Bottom ──────────────────────────────────────
        // Turn360_Bottom → 촬영
        RegisterTagParam("Face_Bottom")
            .Set("FacePosition", 6)
            .Set("Turn180Pos", 0.0)
            .Set("Turn360Pos", 360.0)   // Turn360 Bottom 위치
            .Set("NeedCylOp", 0)
            .Set("CameraZPos", 5.0)
            .Set("BarcodeID", barcodeId)
            .Set("LotID", lotId)
            .Set("SelectCount", 6)
            .End();

        SetParams(ctx);
    }


}
