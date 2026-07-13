#include "pch.h"
#include "CMockSocketConditionVisionProcessor.h"
#include <sstream>

namespace VMF_SOCKET_CONDITION
{
	CMockSocketConditionVisionProcessor::CMockSocketConditionVisionProcessor()
		: m_currentSlotIndex(0)
		, m_startAckPending(false)
		, m_motionPending(false)
		, m_endPending(false)
	{
		InitDefaultMockData();
	}

	CMockSocketConditionVisionProcessor::~CMockSocketConditionVisionProcessor() {}

	void CMockSocketConditionVisionProcessor::InitDefaultMockData()
	{
		AddMockSlotPosition(10.0, 5.0, 0.0);
		AddMockSlotPosition(20.0, 5.0, 0.0);
		AddMockSlotPosition(30.0, 5.0, 0.0);

		AddMockFailSlotLine1(1);
		AddMockFailSlotLine1(3);
	}

	void CMockSocketConditionVisionProcessor::AddMockSlotPosition(
		double x, double y, double z)
	{
		m_mockSlotPositions.push_back({ x, y, z });
	}

	void CMockSocketConditionVisionProcessor::AddMockFailSlotLine1(int slotNo)
	{
		m_mockFailSlotsLine1.push_back(slotNo);
	}

	void CMockSocketConditionVisionProcessor::AddMockFailSlotLine2(int slotNo)
	{
		m_mockFailSlotsLine2.push_back(slotNo);
	}

	// ── 요청 인터페이스 ──────────────────────────────────────────────

	bool CMockSocketConditionVisionProcessor::RequestSetCokAsync(
		const VMF::StringMap& params)
	{
		return true;
	}

	bool CMockSocketConditionVisionProcessor::RequestInspReadyAsync(
		const VMF::StringMap& params)
	{
		m_currentSlotIndex = 0;
		m_startAckPending = true;
		return true;
	}

	bool CMockSocketConditionVisionProcessor::RequestMeasureAsync(
		const VMF::StringMap& params)
	{
		if (m_currentSlotIndex < static_cast<int>(m_mockSlotPositions.size()))
			m_motionPending = true;
		else
			m_endPending = true;
		return true;
	}

	bool CMockSocketConditionVisionProcessor::RequestDeviceCheckAsync(
		const VMF::StringMap& params)
	{
		return true;
	}

	bool CMockSocketConditionVisionProcessor::RequestLightAsync(
		const VMF::StringMap& params)
	{
		return true;
	}

	// ── 수신 콜백 ────────────────────────────────────────────────────

	void CMockSocketConditionVisionProcessor::OnSetCok(VMF::ByteArray body) {}
	void CMockSocketConditionVisionProcessor::OnLight(VMF::ByteArray body) {}

	void CMockSocketConditionVisionProcessor::OnInspReady(VMF::ByteArray body)
	{
		VMF::StringMap data;
		data["StartResult"] = "1";  // 1: OK
		SetLatestData(VMF::InspReady, data);
		SetReceived(VMF::InspReady, true);
	}

	void CMockSocketConditionVisionProcessor::OnMeasure(VMF::ByteArray body)
	{
		if (m_currentSlotIndex >= static_cast<int>(m_mockSlotPositions.size()))
			return;

		const auto& pos = m_mockSlotPositions[m_currentSlotIndex];

		VMF::StringMap data;
		std::ostringstream ox, oy, oz;
		ox << pos.x; data["MotionX"] = ox.str();
		oy << pos.y; data["MotionY"] = oy.str();
		oz << pos.z; data["MotionZ"] = oz.str();

		SetLatestData(VMF::Measure, data);
		SetReceived(VMF::Measure, true);

		m_currentSlotIndex++;
	}

	void CMockSocketConditionVisionProcessor::OnDeviceCheck(VMF::ByteArray body)
	{
		VMF::StringMap data;
		data["MtcTesterPos"] = "0";
		data["ConditionResult"] = m_mockFailSlotsLine1.empty() ? "1" : "2";
		data["FailPositionLine1"] = BuildFailPositionString(m_mockFailSlotsLine1);
		data["FailPositionLine2"] = BuildFailPositionString(m_mockFailSlotsLine2);

		SetLatestData(VMF::DeviceCheck, data);
		SetReceived(VMF::DeviceCheck, true);
	}

	// ── Process ──────────────────────────────────────────────────────

	void CMockSocketConditionVisionProcessor::Process()
	{
		if (m_startAckPending)
		{
			m_startAckPending = false;
			VMF::ByteArray dummy;
			OnInspReady(dummy);

			// Start ACK 후 첫 번째 소켓 좌표 즉시 발행
			m_motionPending = true;
		}

		if (m_motionPending)
		{
			m_motionPending = false;
			VMF::ByteArray dummy;
			OnMeasure(dummy);
		}

		if (m_endPending)
		{
			m_endPending = false;
			VMF::ByteArray dummy;
			OnDeviceCheck(dummy);
		}
	}

	// ── 헬퍼 ─────────────────────────────────────────────────────────

	std::string CMockSocketConditionVisionProcessor::BuildFailPositionString(
		const std::vector<int>& slots)
	{
		if (slots.empty()) return "";

		std::ostringstream oss;
		for (size_t i = 0; i < slots.size(); ++i)
		{
			if (i > 0) oss << ",";
			oss << slots[i];
		}
		return oss.str();
	}

} // namespace VMF_SOCKET_CONDITION