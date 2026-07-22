#include "pch.h"
#include "CSixSideInspectionSequenceBuilder.h"
#include "..\Tasks\CSixSideMoveToFacePositionTask.h"
#include "..\Tasks\CSixSideInspectionTask.h"

using namespace VMF;
using namespace VMF_PLUGIN;

VMF::SequencePtr CSixSideInspectionSequenceBuilder::BuildSequence(const std::string& sequenceName)
{
	VMF::SequencePtr seq(new VMF::Sequence(sequenceName));

	// 공통 상수
	const int    selectCount = 6;
	const int    cameraIndex = 0;
	const double gripperSafetyPos = 0.0;
	const std::string barcodeId = "SN0000001";
	const std::string lotId = "TESTLOT_20240325_1209";

	// ── 면 1: Left ────────────────────────────────────
	{
		auto moveTask = std::make_shared<CSixSideMoveToFacePositionTask>();
		VMF::VisionParams mp;
		mp.visionParams["FacePosition"]     = "1";
		mp.visionParams["Turn180Pos"]       = "10.0";
		mp.visionParams["Turn360Pos"]       = "0.0";
		mp.visionParams["NeedCylOp"]        = "0";
		mp.visionParams["CameraZPos"]       = "5.0";
		mp.visionParams["GripperSafetyPos"] = std::to_string(gripperSafetyPos);
		moveTask->SetTaskParams(mp);
		seq->AddTask(moveTask);
	}
	{
		auto inspTask = std::make_shared<CSixSideInspectionTask>();
		VMF::VisionParams ip;
		ip.visionParams["FacePosition"] = "1";
		ip.visionParams["BarcodeID"] = barcodeId;
		ip.visionParams["LotID"] = lotId;
		ip.visionParams["SelectCount"] = std::to_string(selectCount);
		ip.visionParams["CameraIndex"] = std::to_string(cameraIndex);
		inspTask->SetTaskParams(ip);
		seq->AddTask(inspTask);
	}

	// ── 면 2: Right ───────────────────────────────────
	{
		auto moveTask = std::make_shared<CSixSideMoveToFacePositionTask>();
		VMF::VisionParams mp;
		mp.visionParams["FacePosition"]     = "2";
		mp.visionParams["Turn180Pos"]       = "-10.0";
		mp.visionParams["Turn360Pos"]       = "0.0";
		mp.visionParams["NeedCylOp"]        = "0";
		mp.visionParams["CameraZPos"]       = "5.0";
		mp.visionParams["GripperSafetyPos"] = std::to_string(gripperSafetyPos);
		moveTask->SetTaskParams(mp);
		seq->AddTask(moveTask);
	}
	{
		auto inspTask = std::make_shared<CSixSideInspectionTask>();
		VMF::VisionParams ip;
		ip.visionParams["FacePosition"] = "2";
		ip.visionParams["BarcodeID"] = barcodeId;
		ip.visionParams["LotID"] = lotId;
		ip.visionParams["SelectCount"] = std::to_string(selectCount);
		ip.visionParams["CameraIndex"] = std::to_string(cameraIndex);
		inspTask->SetTaskParams(ip);
		seq->AddTask(inspTask);
	}

	// ── 면 3: Front (NeedCylOp=1: 실린더 전환 필요) ────
	{
		auto moveTask = std::make_shared<CSixSideMoveToFacePositionTask>();
		VMF::VisionParams mp;
		mp.visionParams["FacePosition"] = "3";
		mp.visionParams["Turn180Pos"] = "0.0";
		mp.visionParams["Turn360Pos"] = "90.0";
		mp.visionParams["NeedCylOp"] = "1";
		mp.visionParams["CameraZPos"] = "10.0";
		mp.visionParams["GripperSafetyPos"] = std::to_string(gripperSafetyPos);
		moveTask->SetTaskParams(mp);
		seq->AddTask(moveTask);
	}
	{
		auto inspTask = std::make_shared<CSixSideInspectionTask>();
		VMF::VisionParams ip;
		ip.visionParams["FacePosition"] = "3";
		ip.visionParams["BarcodeID"] = barcodeId;
		ip.visionParams["LotID"] = lotId;
		ip.visionParams["SelectCount"] = std::to_string(selectCount);
		ip.visionParams["CameraIndex"] = std::to_string(cameraIndex);
		inspTask->SetTaskParams(ip);
		seq->AddTask(inspTask);
	}

	// ── 면 4: Top ─────────────────────────────────────
	{
		auto moveTask = std::make_shared<CSixSideMoveToFacePositionTask>();
		VMF::VisionParams mp;
		mp.visionParams["FacePosition"] = "4";
		mp.visionParams["Turn180Pos"] = "0.0";
		mp.visionParams["Turn360Pos"] = "180.0";
		mp.visionParams["NeedCylOp"] = "0";
		mp.visionParams["CameraZPos"] = "15.0";
		mp.visionParams["GripperSafetyPos"] = std::to_string(gripperSafetyPos);
		moveTask->SetTaskParams(mp);
		seq->AddTask(moveTask);
	}
	{
		auto inspTask = std::make_shared<CSixSideInspectionTask>();
		VMF::VisionParams ip;
		ip.visionParams["FacePosition"] = "4";
		ip.visionParams["BarcodeID"] = barcodeId;
		ip.visionParams["LotID"] = lotId;
		ip.visionParams["SelectCount"] = std::to_string(selectCount);
		ip.visionParams["CameraIndex"] = std::to_string(cameraIndex);
		inspTask->SetTaskParams(ip);
		seq->AddTask(inspTask);
	}

	// ── 면 5: Rear ────────────────────────────────────
	{
		auto moveTask = std::make_shared<CSixSideMoveToFacePositionTask>();
		VMF::VisionParams mp;
		mp.visionParams["FacePosition"] = "5";
		mp.visionParams["Turn180Pos"] = "0.0";
		mp.visionParams["Turn360Pos"] = "270.0";
		mp.visionParams["NeedCylOp"] = "0";
		mp.visionParams["CameraZPos"] = "10.0";
		mp.visionParams["GripperSafetyPos"] = std::to_string(gripperSafetyPos);
		moveTask->SetTaskParams(mp);
		seq->AddTask(moveTask);
	}
	{
		auto inspTask = std::make_shared<CSixSideInspectionTask>();
		VMF::VisionParams ip;
		ip.visionParams["FacePosition"] = "5";
		ip.visionParams["BarcodeID"] = barcodeId;
		ip.visionParams["LotID"] = lotId;
		ip.visionParams["SelectCount"] = std::to_string(selectCount);
		ip.visionParams["CameraIndex"] = std::to_string(cameraIndex);
		inspTask->SetTaskParams(ip);
		seq->AddTask(inspTask);
	}

	// ── 면 6: Bottom ──────────────────────────────────
	{
		auto moveTask = std::make_shared<CSixSideMoveToFacePositionTask>();
		VMF::VisionParams mp;
		mp.visionParams["FacePosition"]     = "6";
		mp.visionParams["Turn180Pos"]       = "0.0";
		mp.visionParams["Turn360Pos"]       = "360.0";
		mp.visionParams["NeedCylOp"]        = "0";
		mp.visionParams["CameraZPos"]       = "5.0";
		mp.visionParams["GripperSafetyPos"] = std::to_string(gripperSafetyPos);
		moveTask->SetTaskParams(mp);
		seq->AddTask(moveTask);
	}
	{
		auto inspTask = std::make_shared<CSixSideInspectionTask>();
		VMF::VisionParams ip;
		ip.visionParams["FacePosition"] = "6";
		ip.visionParams["BarcodeID"]    = barcodeId;
		ip.visionParams["LotID"]        = lotId;
		ip.visionParams["SelectCount"]  = std::to_string(selectCount);
		ip.visionParams["CameraIndex"]  = std::to_string(cameraIndex);
		inspTask->SetTaskParams(ip);
		seq->AddTask(inspTask);
	}

	return VMF::SequencePtr(seq.release());
}
