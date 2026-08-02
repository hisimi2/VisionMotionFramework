#pragma once
#include "NonBlockingTaskBase.h"

namespace VMF_PLUGIN
{
	// ----------------------------------------------------------------
	// CSixSideMoveToFacePositionTask
	//
	// 각 면 검사 전 모듈을 해당 면이 카메라 앞에 오도록 회전.
	//
	// Tag 파라미터:
	//   "FacePosition"  ? 면 번호 (1~6)
	//   "Turn180Pos"    ? Turn180 목표 위치 (Left/Right/Safety)
	//   "Turn360Pos"    ? Turn360 목표 위치 (None/Front/Top/Rear/Bottom)
	//   "NeedCylOp"     ? 실린더 동작 필요 여부 (0/1)
	//   "CameraZPos"    ? CameraZ 목표 위치
	//
	// 실린더 동작(NeedCylOp=1):
	//   Front/Top/Rear/Bottom 면 진입 시
	//   Gripper → 실린더 클램프 전환 후 Turn360 회전
	// ----------------------------------------------------------------
	class CSixSideMoveToFacePositionTask : public VMF::NonBlockingTaskBase
	{
		enum Substep
		{
			Idle = 0,
			MoveTurn180,            // Turn180 축 이동
			WaitTurn180,
			DoCylGrip,              // 실린더 클램프 (NeedCylOp=1)
			WaitCylGrip,
			DoGripperSafety,        // Gripper Safety 이동
			WaitGripperSafety,
			DoCylUngrip,            // 실린더 언클램프
			WaitCylUngrip,
			DoCylBack,              // 실린더 후진
			WaitCylBack,
			MoveTurn360,            // Turn360 축 이동
			WaitTurn360,
			MoveCameraZ,            // CameraZ 이동
			WaitCameraZ,
			Done,
		};

		VMF::TaskResult HandleMoveTurn180(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleWaitTurn180(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleDoCylGrip(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleWaitCylGrip(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleDoGripperSafety(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleWaitGripperSafety(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleDoCylUngrip(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleWaitCylUngrip(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleDoCylBack(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleWaitCylBack(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleMoveTurn360(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleWaitTurn360(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleMoveCameraZ(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleWaitCameraZ(VMF::Context& ctx, VMF::IActuator* actuator);

public:
		explicit CSixSideMoveToFacePositionTask(VMF::IActuator* actuator=nullptr);
		~CSixSideMoveToFacePositionTask() override;

		std::string GetName() const override
		{
			return "Task_6Side_MoveToFacePosition";
		}

	protected:
		void            OnInitialize(VMF::Context& ctx) override;
		VMF::TaskResult OnPoll(VMF::Context& ctx,
			VMF::IActuator* actuator) override;

	private:
		bool   m_needCylOp;
		double m_turn180Pos;
		double m_turn360Pos;
		double m_cameraZPos;
		long   m_moveTimeoutMs;
		long   m_cylTimeoutMs;
	};
} 
