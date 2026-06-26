#include "StdAfx.h"
#include "VatAdapterLoad1.h"

// Load1 축 인덱스 (pos 벡터 순서)
//   pos[0] = LOAD1_X
//   pos[1] = LOAD1_Y
//   pos[2] = LOAD1_Z   (MoveZ 전용이지만 Move에서도 참조 가능)
//   pos[3] = TABLE1
//   pos[4] = TABLE2

namespace VMF_Load1
{
	using namespace VMF;

	static const char* AXIS_X = "X";
	static const char* AXIS_Y = "Y";
	static const char* AXIS_Z = "Z";
	static const char* AXIS_TABLE1 = "TABLE1";
	static const char* AXIS_TABLE2 = "TABLE2";

	VatAdapterLoad1::VatAdapterLoad1(Load1Parts* parts)
		: m_parts(parts) {}

	VatAdapterLoad1::~VatAdapterLoad1() {}

	// ── Pitch ──────────────────────────────────────────────────────────
	PitchType VatAdapterLoad1::GetPitchType() { return Variable; }
	ActError  VatAdapterLoad1::IsReadyToMove() { return ActOk; }

	// ── MoveZ ──────────────────────────────────────────────────────────
	ActError VatAdapterLoad1::MoveZ(double targetZ)
	{
		if (!m_parts) return ActFail;
		m_parts->AxisZ.Move(targetZ);
		return ActOk;
	}

	ActError VatAdapterLoad1::isMoveZ(double targetZ)
	{
		if (!m_parts) return ActFail;
		double diff = std::abs(m_parts->AxisZ.GetEncoder() - targetZ);
		return (diff <= 1.0) ? ActOk : ActFail;
	}

	// ── Move (MotionCommand 기반) ─────────────────────────────────────
	ActError VatAdapterLoad1::Move(MotionCommand& cmd)
	{
		if (!m_parts) return ActFail;

		// X, Y 이동 (X/Y 명령이 있을 때만)
		if (cmd.Exist(AXIS_X)) m_parts->AxisX.Move(cmd.Get(AXIS_X).value().targetPos.value_or(0.0));
		if (cmd.Exist(AXIS_Y)) m_parts->AxisY.Move(cmd.Get(AXIS_Y).value().targetPos.value_or(0.0));

		// Pitch 변경
		if (cmd.pitch == Narrow)
			m_parts->CylYPitch.narrow(true);
		else
			m_parts->CylYPitch.wide(true);

		// Table 이동 (명령이 있으면 해당 위치, 없으면 퇴피 위치 100)
		if (cmd.Exist(AXIS_TABLE1))
			m_parts->AxisTable1.Move(cmd.Get(AXIS_TABLE1).value().targetPos.value_or(100.0));
		else
			m_parts->AxisTable1.Move(100.0);

		if (cmd.Exist(AXIS_TABLE2))
			m_parts->AxisTable2.Move(cmd.Get(AXIS_TABLE2).value().targetPos.value_or(100.0));
		else
			m_parts->AxisTable2.Move(100.0);

		m_parts->CylBuffer.backward(true);
		return ActOk;
	}

	ActError VatAdapterLoad1::isMove(MotionCommand& cmd)
	{
		if (!m_parts) return ActFail;

		// X, Y 도달 확인
		if (cmd.Exist(AXIS_X))
		{
			double target = cmd.Get(AXIS_X).value().targetPos.value_or(0.0);
			if (std::abs(m_parts->AxisX.GetEncoder() - target) > 1.0)
				return ActFail;
		}
		if (cmd.Exist(AXIS_Y))
		{
			double target = cmd.Get(AXIS_Y).value().targetPos.value_or(0.0);
			if (std::abs(m_parts->AxisY.GetEncoder() - target) > 1.0)
				return ActFail;
		}

		// Pitch 확인
		if (cmd.pitch == Narrow && !m_parts->CylYPitch.isNarrow())
			return ActFail;
		if (cmd.pitch == Wide && !m_parts->CylYPitch.isWide())
			return ActFail;

		// Table 도달 확인
		if (cmd.Exist(AXIS_TABLE1))
		{
			double target = cmd.Get(AXIS_TABLE1).value().targetPos.value_or(100.0);
			if (std::abs(m_parts->AxisTable1.GetEncoder() - target) > 1.0)
				return ActFail;
		}
		if (cmd.Exist(AXIS_TABLE2))
		{
			double target = cmd.Get(AXIS_TABLE2).value().targetPos.value_or(100.0);
			if (std::abs(m_parts->AxisTable2.GetEncoder() - target) > 1.0)
				return ActFail;
		}

		// Buffer 실린더 확인
		if (!m_parts->CylBuffer.isBackward())
			return ActFail;

		return ActOk;
	}

	ActError VatAdapterLoad1::Stop()
	{
		if (!m_parts) return ActFail;
		/*m_parts->AxisX.Stop();
		m_parts->AxisY.Stop();
		m_parts->AxisZ.Stop();
		m_parts->AxisTable1.Stop();
		m_parts->AxisTable2.Stop();*/
		return ActOk;
	}

	// ── Position / Pulse ───────────────────────────────────────────────
	std::vector<double> VatAdapterLoad1::getPosition()
	{
		return{
			m_parts->AxisX.GetEncoder(),
			m_parts->AxisY.GetEncoder(),
			m_parts->AxisZ.GetEncoder()
		};
	}

	std::vector<double> VatAdapterLoad1::getPulse()
	{
		return{
			m_parts->AxisX.GetEncoder(),
			m_parts->AxisY.GetEncoder(),
			m_parts->AxisTable1.GetEncoder(),
			m_parts->AxisTable2.GetEncoder()
		};
	}

	// ── Light ──────────────────────────────────────────────────────────
	int VatAdapterLoad1::SetLightState(int camIndex, bool on)
	{
		if (!m_parts) return 0;
		switch (camIndex)
		{
		case 0: m_parts->LampLeft.SetStatus(on);  break;
		case 1: m_parts->LampRight.SetStatus(on); break;
		case 2: m_parts->LampLower.SetStatus(on); break;
		default: return 0;
		}
		return 1;
	}

	int VatAdapterLoad1::GetLightState(int camIndex, bool& outOn)
	{
		if (!m_parts) return 0;
		switch (camIndex)
		{
		case 0: outOn = m_parts->LampLeft.GetStatus();  break;
		case 1: outOn = m_parts->LampRight.GetStatus(); break;
		case 2: outOn = m_parts->LampLower.GetStatus(); break;
		default: return 0;
		}
		return 1;
	}

	// ── Laser (미구현 — Load1 HW 없음) ───────────────────────────────
	ActError VatAdapterLoad1::SetLaserState(int /*ch*/, bool /*on*/)
	{
		return ActFail;  // Load1에는 레이저 없음
	}

	ActError VatAdapterLoad1::GetLaserState(int /*ch*/, bool& outOn)
	{
		outOn = false;
		return ActFail;
	}

	// ── Trigger (미구현) ───────────────────────────────────────────────
	ActError VatAdapterLoad1::SetTriggerState(bool /*enable*/, double /*intervalMm*/)
	{
		return ActFail;
	}

	ActError VatAdapterLoad1::GetTriggerState(bool& outEnabled, double& outIntervalMm)
	{
		outEnabled = false;
		outIntervalMm = 0.0;
		return ActFail;
	}

} // namespace VMF_Load1