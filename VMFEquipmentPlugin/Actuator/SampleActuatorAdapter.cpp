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
		// Sample: 항상 Variable 반환 (PitchType: Fixed/Variable)
		// Load1 예시: m_parts->GetCurrentPitchType()
		// PitchStatus(Narrow/Wide)가 아닌 PitchType(Fixed/Variable) 반환
		return VMF::Variable;
	}

	// !!! 수정 필요: 장비의 모든 안전 조건 검사 !!!
	// 예: Door Closed, Air Pressure OK, Gripper Released, Stage Locked 등
	VMF::ActError SampleActuatorAdapter::IsReadyToMove()
	{
		// Sample: 항상 준비됨 반환
		// Load1 예시:
		//   if (!m_parts->IsDoorClosed()) return VMF::ActError_Safety;
		//   if (!m_parts->IsAirPressureOK()) return VMF::ActError_Safety;
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

	VMF::ActError SampleActuatorAdapter::Stop()
	{
		// !!! 수정 필요: 장비의 비상정지/이동 중단 호출 !!!
		// Load1 예시: m_parts->StopAllAxes();

		// [SAMPLE] 항상 성공 가정
		return VMF::ActOk;
	}

	std::vector<double> SampleActuatorAdapter::getPosition()
	{
		// !!! 수정 필요: 장비의 현재 위치 반환 !!!
		// Load1 예시:
		//   std::vector<double> pos;
		//   pos.push_back(m_parts->GetAxisPosition(AXIS_X));
		//   pos.push_back(m_parts->GetAxisPosition(AXIS_Y));
		//   pos.push_back(m_parts->GetAxisPosition(AXIS_Z));
		//   return pos;

		// [SAMPLE] 빈 벡터 반환
		return std::vector<double>();
	}

	std::vector<double> SampleActuatorAdapter::getPulse()
	{
		// !!! 수정 필요: 장비의 현재 펄스 위치 반환 !!!
		// [SAMPLE] 빈 벡터 반환
		return std::vector<double>();
	}

	int SampleActuatorAdapter::SetLightState(int cameraId, bool on)
	{
		// !!! 수정 필요: 장비의 조명 제어 호출 !!!
		// Load1 예시:
		//   if (on) m_parts->TurnOnLight(cameraId);
		//   else    m_parts->TurnOffLight(cameraId);

		// [SAMPLE] 로그만 출력, 성공 시 0 반환
		// Logger::Info("SetLightState: Camera=%d, On=%d", cameraId, (int)on);
		return 0;  // 0 = 성공
	}

	int SampleActuatorAdapter::GetLightState(int camIndex, bool& outOn)
	{
		// !!! 수정 필요: 장비의 조명 상태 조회 !!!
		// [SAMPLE] 항상 false 반환
		outOn = false;
		return 0;
	}

	VMF::ActError SampleActuatorAdapter::SetLaserState(int laserChannel, bool on)
	{
		// !!! 수정 필요: 장비의 레이저 제어 호출 !!!
		(void)laserChannel;
		(void)on;
		return VMF::ActOk;
	}

	VMF::ActError SampleActuatorAdapter::GetLaserState(int laserChannel, bool& outOn)
	{
		// !!! 수정 필요: 장비의 레이저 상태 조회 !!!
		(void)laserChannel;
		outOn = false;
		return VMF::ActOk;
	}

	VMF::ActError SampleActuatorAdapter::SetTriggerState(bool enable, double intervalMm)
	{
		// !!! 수정 필요: 장비의 트리거 제어 호출 !!!
		(void)enable;
		(void)intervalMm;
		return VMF::ActOk;
	}

	VMF::ActError SampleActuatorAdapter::GetTriggerState(bool& outEnabled, double& outIntervalMm)
	{
		// !!! 수정 필요: 장비의 트리거 상태 조회 !!!
		outEnabled = false;
		outIntervalMm = 0.0;
		return VMF::ActOk;
	}
} // namespace VMF_Sample
