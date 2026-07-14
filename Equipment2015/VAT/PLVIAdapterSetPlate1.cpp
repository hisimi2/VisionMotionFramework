#include "stdafx.h"
#include "PLVIAdapterSetPlate1.h"

// SetPlate1 PLVI 축 구성
//   Y축 : 스캔 방향 (PLVI 촬영 이동)
//   Z축 : 안전 높이 / Vision 촬영 높이 제어

namespace VMF
{
	static const char* AXIS_Y = "Y";
	static const char* AXIS_Z = "Z";

	PLVIAdapterSetPlate1::PLVIAdapterSetPlate1(SetPlate1Parts* parts)
		: m_parts(parts)
		, m_triggerEnabled(false)
		, m_triggerIntervalMm(0.0)
		, m_scanDistMm(315.0)    // SetPlate1 기준 (Shuttle=532.0)
		, m_accelDistMm(10.0)    // 가속 여유 거리 (모든 검사 위치에서 공통적으로 10 사용)
	{}

	PLVIAdapterSetPlate1::~PLVIAdapterSetPlate1() {}

	PitchType PLVIAdapterSetPlate1::GetPitchType() { return Fixed; }
	ActError  PLVIAdapterSetPlate1::IsReadyToMove() { return m_parts ? ActOk : ActFail; }


	ActError PLVIAdapterSetPlate1::MoveZ(double targetZ)
	{
		if (!m_parts) return ActFail;
		m_parts->AxisZ.Move(targetZ);
		return ActOk;
	}

	ActError PLVIAdapterSetPlate1::isMoveZ(double targetZ)
	{
		if (!m_parts) return ActFail;
		double diff = std::abs(m_parts->AxisZ.GetEncoder() - targetZ);
		return (diff <= 1.0) ? ActOk : ActWait;
	}

	ActError PLVIAdapterSetPlate1::Move(MotionCommand& cmd)
	{
		if (!m_parts) return ActFail;

		if (cmd.Exist(AXIS_Y))
		{
//			double targetY = cmd.Get(AXIS_Y).value().targetPos;

			// 스캔 속도 설정
			// if (cmd.Get(AXIS_Y).value().speed.has_value())
			//     m_parts->AxisY.SetSpeed(cmd.Get(AXIS_Y).value().speed.value());

//			m_parts->AxisY.Move(targetY);
		}

		return ActOk;
	}

	ActError PLVIAdapterSetPlate1::isMove(MotionCommand& cmd)
	{
		if (!m_parts) return ActFail;

		if (cmd.Exist(AXIS_Y))
        {
            /*
			double target = cmd.Get(AXIS_Y).value().targetPos;
			if (std::abs(m_parts->AxisY.GetEncoder() - target) > 1.0)
				return ActWait;
            */
		}

		return ActOk;
	}

	ActError PLVIAdapterSetPlate1::Stop()
	{
		if (!m_parts) return ActFail;
		// m_parts->AxisY.Stop();
		// m_parts->AxisZ.Stop();
		return ActOk;
	}

	int PLVIAdapterSetPlate1::SetLightState(int camIndex, bool on)
	{
		if (!m_parts) return 0;
		switch (camIndex)
		{
		case 0: m_parts->LampPLVI.SetStatus(on); break;
		case 1: m_parts->LampSide.SetStatus(on); break;
		default: return 0;
		}
		return 1;
	}

	int PLVIAdapterSetPlate1::GetLightState(int camIndex, bool& outOn)
	{
		if (!m_parts) return 0;
		switch (camIndex)
		{
		case 0: outOn = m_parts->LampPLVI.GetStatus(); break;
		case 1: outOn = m_parts->LampSide.GetStatus(); break;
		default: return 0;
		}
		return 1;
	}


	// Task는 enable/intervalMm만 전달
	// scanDistMm, accelDistMm은 이 Adapter가 위치에 맞게 보유
	//
	// 실제 HW 연결 시
	//   g_Motor[eAXIS_PLVI_MOD_1]->SetPlviCFG_REG(
	//       enable,
	//       m_scanDistMm  * 1000.0,   // mm → um
	//       m_accelDistMm * 1000.0,   // mm → um
	//       intervalMm    * 1000.0    // mm → um
	//   );
	ActError PLVIAdapterSetPlate1::SetTriggerState(bool enable, double intervalMm)
	{
		if (!m_parts) return ActFail;

		m_triggerEnabled = enable;
		m_triggerIntervalMm = intervalMm;

		// TODO: 실제 모션 컨트롤러 트리거 API 연결
		//
		// [AXT / 동양 계열 모션 컨트롤러 예시]
		// g_Motor[eAXIS_PLVI_MOD_1]->SetPlviCFG_REG(
		//     enable,
		//     m_scanDistMm  * 1000.0,   // 촬영 구간 (um 단위)
		//     m_accelDistMm * 1000.0,   // 가속 여유 거리 (um 단위)
		//     intervalMm    * 1000.0    // 트리거 간격 (um 단위)
		// );
		//
		// [DIO 보드 방식 예시 (NI DAQ 등)]
		// if (enable)
		// {
		//     m_parts->TriggerBoard.SetEncoderSource(AXIS_Y_ENCODER);
		//     m_parts->TriggerBoard.SetInterval(intervalMm * 1000.0);  // um
		//     m_parts->TriggerBoard.SetScanDist(m_scanDistMm * 1000.0);
		//     m_parts->TriggerBoard.SetAccelDist(m_accelDistMm * 1000.0);
		//     m_parts->TriggerBoard.Enable();
		// }
		// else
		// {
		//     m_parts->TriggerBoard.Disable();
		// }

		return ActOk;
	}

	ActError PLVIAdapterSetPlate1::GetTriggerState(bool& outEnabled, double& outIntervalMm)
	{
		outEnabled = m_triggerEnabled;
		outIntervalMm = m_triggerIntervalMm;
		return ActOk;
	}

	ActError PLVIAdapterSetPlate1::ClampSetplate(int idx, bool clamp)
	{
		if (!m_parts) return ActFail;
		if (idx < 0 || idx >= static_cast<int>(m_parts->CylSetplateClamp.size()))
			return ActInvalidParam;

		if (clamp)
			m_parts->CylSetplateClamp[idx].clamp(true);
		else
			m_parts->CylSetplateClamp[idx].release(true);

		return ActOk;
	}

	ActError PLVIAdapterSetPlate1::IsSetplateClamped(int idx) const
	{
		if (!m_parts) return ActFail;
		if (idx < 0 || idx >= static_cast<int>(m_parts->CylSetplateClamp.size()))
			return ActInvalidParam;

		return m_parts->CylSetplateClamp[idx].isClamp() ? ActOk : ActWait;
	}

	ActError PLVIAdapterSetPlate1::LiftSetplate(int idx, bool up)
	{
		if (!m_parts) return ActFail;
		if (idx < 0 || idx >= static_cast<int>(m_parts->CylSetplateUpDown.size()))
			return ActInvalidParam;

		if (up)
			m_parts->CylSetplateUpDown[idx].up(true);
		else
			m_parts->CylSetplateUpDown[idx].down(true);

		return ActOk;
	}

	ActError PLVIAdapterSetPlate1::IsSetplateLift(int idx) const
	{
		if (!m_parts) return ActFail;
		if (idx < 0 || idx >= static_cast<int>(m_parts->CylSetplateUpDown.size()))
			return ActInvalidParam;

		return m_parts->CylSetplateUpDown[idx].isUp() ? ActOk : ActWait;
	}
} 
