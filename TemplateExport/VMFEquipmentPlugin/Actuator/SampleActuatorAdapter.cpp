#include "pch.h"
#include "SampleActuatorAdapter.h"

// [SAMPLE] 축 인덱스 (pos 벡터 순서)
//   pos[0] = X
//   pos[1] = Y
//   pos[2] = Z   (MoveZ 전용이지만 Move에서도 참조 가능)
//   pos[3] = TABLE1
//   pos[4] = TABLE2

namespace VMF_Sample
{
	using namespace VMF;

	static const char* AXIS_X = "X";
	static const char* AXIS_Y = "Y";
	static const char* AXIS_Z = "Z";
	static const char* AXIS_TABLE1 = "TABLE1";
	static const char* AXIS_TABLE2 = "TABLE2";

	// !!! 수정 필요: 생성자는 장비의 실제 Parts 객체를 받도록 수정 !!!
	// SampleActuatorAdapter::SampleActuatorAdapter(Load1Parts* parts)
	//     : m_parts(parts)
	// {}
	SampleActuatorAdapter::SampleActuatorAdapter()
	{
	}

	SampleActuatorAdapter::~SampleActuatorAdapter()
	{
	}

	// !!! 수정 필요: 장비의 PitchType 반환 로직으로 변경 !!!
	VMF::PitchType SampleActuatorAdapter::GetPitchType()
	{
		// Sample: 항상 Narrow 반환
		// Load1 예시: m_parts->GetCurrentPitchType()
		return VMF::Narrow;
	}

	// !!! 수정 필요: 장비의 모든 안전 조건 검사 !!!
	// 예: Door Closed, Air Pressure OK, Gripper Released, Stage Locked 등
	VMF::ActError SampleActuatorAdapter::IsReadyToMove()
	{
		// Sample: 항상 준비됨 반환
		// Load1 예시:
		//   if (!m_parts->IsDoorClosed()) return ActError_Safety;
		//   if (!m_parts->IsAirPressureOK()) return ActError_Safety;
		return VMF::ActOk;
	}

	VMF::ActError SampleActuatorAdapter::MoveZ(double targetZ)
	{
		// !!! 수정 필요: 장비의 실제 Z축 모션 제어 호출 !!!
		// Load1 예시: m_parts->MoveAxis(AXIS_Z, targetZ, SPEED_HIGH);
		// 반환: 성공 시 ActOk, 실패 시 ActError_MotionFailed

		// [SAMPLE] 로그 출력 후 성공 가정
		// Logger::Info("MoveZ: %f", targetZ);
		return VMF::ActOk;
	}

	VMF::ActError SampleActuatorAdapter::Move(VMF::MotionCommand& cmd)
	{
		// !!! 수정 필요: 장비의 실제 다축 모션 제어 호출 !!!
		// Load1 예시:
		//   if (cmd.Has(AXIS_X)) m_parts->MoveAxis(AXIS_X, cmd.Get(AXIS_X), cmd.pitch);
		//   if (cmd.Has(AXIS_Y)) m_parts->MoveAxis(AXIS_Y, cmd.Get(AXIS_Y), cmd.pitch);
		//   if (cmd.Has(AXIS_TABLE1)) m_parts->MoveAxis(AXIS_TABLE1, cmd.Get(AXIS_TABLE1));
		//   if (cmd.Has(AXIS_TABLE2)) m_parts->MoveAxis(AXIS_TABLE2, cmd.Get(AXIS_TABLE2));

		// [SAMPLE] 로그만 출력
		// Logger::Info("Move: X=%f, Y=%f, Pitch=%d",
		//     cmd.Get(AXIS_X), cmd.Get(AXIS_Y), (int)cmd.pitch);
		return VMF::ActOk;
	}

	VMF::ActError SampleActuatorAdapter::isMoveZ(double targetZ)
	{
		// !!! 수정 필요: 장비의 Z축 도달 여부 확인 !!!
		// Load1 예시:
		//   if (m_parts->IsAxisInPosition(AXIS_Z, targetZ, POSITION_TOLERANCE))
		//       return ActOk;
		//   return ActError_Busy;

		// [SAMPLE] 항상 도달했다고 가정
		return VMF::ActOk;
	}

	VMF::ActError SampleActuatorAdapter::isMove(VMF::MotionCommand& cmd)
	{
		// !!! 수정 필요: 장비의 다축 도달 여부 확인 !!!
		// Load1 예시:
		//   if (cmd.Has(AXIS_X) && !m_parts->IsAxisInPosition(AXIS_X, cmd.Get(AXIS_X))) return ActError_Busy;
		//   if (cmd.Has(AXIS_Y) && !m_parts->IsAxisInPosition(AXIS_Y, cmd.Get(AXIS_Y))) return ActError_Busy;

		// [SAMPLE] 항상 도달했다고 가정
		return VMF::ActOk;
	}

	VMF::ActError SampleActuatorAdapter::SetLightState(int cameraId, bool on)
	{
		// !!! 수정 필요: 장비의 조명 제어 호출 !!!
		// Load1 예시:
		//   if (on) m_parts->TurnOnLight(cameraId);
		//   else    m_parts->TurnOffLight(cameraId);

		// [SAMPLE] 로그만 출력
		// Logger::Info("SetLightState: Camera=%d, On=%d", cameraId, (int)on);
		return VMF::ActOk;
	}
} // namespace VMF_Sample
