#pragma once
#include "VisionProcessorBase.h"
#include <vector>
#include <string>
#include <map>

namespace VMF_SOCKET_CONDITION
{
	// ----------------------------------------------------------------
	// CMockSocketConditionVisionProcessor
	//
	// Socket Condition 검사 프로토콜 Mock 구현.
	//
	// 실제 Vision PC 없이 시퀀스 동작 검증 용도.
	//
	// 시뮬레이션 흐름:
	//   1104 InspReady 수신 → Start OK 응답
	//   1105 Measure 수신   → 소켓별 좌표를 순서대로 발행
	//                         (미리 등록된 MockSlotPositions 사용)
	//   1106 DeviceCheck    → 전체 결과 발행
	//                         (미리 등록된 MockFailSlots 사용)
	// ----------------------------------------------------------------
	class CMockSocketConditionVisionProcessor : public VMF::VisionProcessorBase
	{
	public:
		CMockSocketConditionVisionProcessor();
		~CMockSocketConditionVisionProcessor() override;

		// ── 요청 인터페이스 ───────────────────────────────────────────
		bool RequestSetCokAsync(const VMF::StringMap& params) override;
		bool RequestInspReadyAsync(const VMF::StringMap& params) override; // 1104
		bool RequestMeasureAsync(const VMF::StringMap& params) override; // 1105 ACK
		bool RequestDeviceCheckAsync(const VMF::StringMap& params) override; // 1106 ACK
		bool RequestLightAsync(const VMF::StringMap& params) override;

		// ── 수신 콜백 ─────────────────────────────────────────────────
		void OnSetCok(VMF::ByteArray body) override;
		void OnInspReady(VMF::ByteArray body) override; // 1104 응답 수신
		void OnMeasure(VMF::ByteArray body) override; // 1105 좌표 수신
		void OnDeviceCheck(VMF::ByteArray body) override; // 1106 결과 수신
		void OnLight(VMF::ByteArray body) override;

		void Process() override;

		// ── Mock 설정 ─────────────────────────────────────────────────
		// 소켓별 이동 좌표 등록 (x, y, z 순서)
		void AddMockSlotPosition(double x, double y, double z);

		// 불량 소켓 번호 등록 (Line1)
		void AddMockFailSlotLine1(int slotNo);

		// 불량 소켓 번호 등록 (Line2)
		void AddMockFailSlotLine2(int slotNo);

	private:
		struct SlotPosition
		{
			double x, y, z;
		};

		std::vector<SlotPosition> m_mockSlotPositions;
		std::vector<int>          m_mockFailSlotsLine1;
		std::vector<int>          m_mockFailSlotsLine2;
		int                       m_currentSlotIndex;
		bool                      m_startAckPending;
		bool                      m_motionPending;
		bool                      m_endPending;

		// 콤마 구분 문자열 생성
		std::string BuildFailPositionString(const std::vector<int>& slots);

		// Mock 기본 소켓 위치 초기화 (3개 소켓 예시)
		void InitDefaultMockData();
	};

} // namespace VMF_SOCKET_CONDITION