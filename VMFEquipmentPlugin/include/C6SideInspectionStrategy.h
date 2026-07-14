#pragma once
#include "ComponentSetupBase.h"
#include "SqliteDataRepository.h"
#include "scr\Protocol\Vision6SideProcessor.h"
#include "scr\Sequences\C6SideInspectionSequenceBuilder.h"

namespace VMF_6SIDE
{
	// ----------------------------------------------------------------
	// C6SideInspectionStrategy
	//
	// 6면 검사 Strategy (Loader 기준).
	//
	// 면별 Tag 파라미터:
	//   Turn180Pos   ? Turn180 목표 위치 (double, 실제 레시피 좌표)
	//   Turn360Pos   ? Turn360 목표 위치 (double, 실제 레시피 좌표)
	//   NeedCylOp    ? 실린더 전환 필요 여부 (0/1)
	//   CameraZPos   ? CameraZ 목표 위치 (double)
	//   FacePosition ? 1102 Data[0] 값 (1~6)
	//   BarcodeID    ? 모듈 S/N
	//   LotID        ? Lot ID
	//   SelectCount  ? 검사할 면 총 개수 (6면 전체 = 6)
	//   CameraIndex  ? Vision CamIndex
	// ----------------------------------------------------------------
	class C6SideInspectionStrategy : public VMF::ComponentSetupBase
	{
	public:
		std::string GetSequenceName() const override
		{
			return "6SideInspection";
		}

		VMF::DataRepositoryPtr CreateRepository() override
		{
			auto repo = std::make_shared<VMF::SqliteDataRepository>(
				"Data\\6SIDE_DATABASE.db", "Data\\Images");
			repo->Initialize();
			return repo;
		}

		VMF::VisionProcessorPtr CreateVisionProcessor() override
		{
			VMF::VisionConnectionConfig config("127.0.0.1", 8001, 3000);
			auto vm = std::make_shared<VMF::Vision6SideProcessor>();
			vm->Initialize(config);
			return vm;
		}

		VMF::SequenceBuilderPtr CreateBuilder() override
		{
			return std::make_shared<
				VMF_6SIDE::Sequence::C6SideInspectionSequenceBuilder>();
		}

		void ConfigureParams(VMF::VisionContextPtr ctx) override
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
	};

} // namespace VMF_6SIDE
