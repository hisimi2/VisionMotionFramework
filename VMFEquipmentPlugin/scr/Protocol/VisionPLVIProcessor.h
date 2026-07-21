#pragma once
#include "VisionCommunicationManager.h"
#include "VisionMemoryKeys.h"
#include "VisionPacketMemory.h"
#include "VMFEquipmentPluginExport.h"

namespace VMF
{
	// ================================================================
	// VisionPlviProcessor
	//
	// PLVI 전용 VisionProcessor.
	// S107/F5 (REQ_MEASURE) 로 검사 시작 및 결과 요청.
	// S107/F6 (REQ_RESULT)  로 응답 수신.
	//
	// VisionCommand 매핑:
	//   Measure   → 검사 시작 요청 (1차 REQ_MEASURE)
	//   InspReady → 결과 요청     (2차 REQ_MEASURE)
	// ================================================================
	namespace VisionPLVIProtocol
	{
		// PLVI는 S107/F5, S107/F6 사용
		enum class Stream : int { Measure = 107 };
		enum class Function : int
		{
			Request = 5,   // H→V REQ_MEASURE
			Result = 6    // V→H REQ_RESULT
		};
		static const VC::VisionProtocolId PLVIRequest(
			static_cast<int>(Stream::Measure),
			static_cast<int>(Function::Request));
		static const VC::VisionProtocolId PLVIResult(
			static_cast<int>(Stream::Measure),
			static_cast<int>(Function::Result));
	}

    class VMF_PLUGIN_API VisionPlviProcessor : public VisionCommunicationManager
	{
	public:
		VisionPlviProcessor();
		~VisionPlviProcessor() override;

		// ── Request 함수 ─────────────────────────────────────────────
		// 검사 시작 요청 (1차 REQ_MEASURE, S107/F5)
		// params 필요 키:
		//   PLVI_POSITION, PLVI_PKG_NAME, PLVI_CTRAY_X, PLVI_CTRAY_Y,
		//   PLVI_DEVICE_INFO ("0,99,99,0,..." 콤마 구분)
		bool RequestMeasureAsync(const StringMap& params) override;

		// 결과 요청 (2차 REQ_MEASURE, S107/F5)
		// params: 필요 없음 (nDataID만 전송)
		bool RequestInspReadyAsync(const StringMap& params) override;

		// 미사용
		bool RequestSetCokAsync(const StringMap& params) override;
		bool RequestDeviceCheckAsync(const StringMap& params) override;
		bool RequestLightAsync(const StringMap& params) override;

		// ── On 함수 (수신 콜백) ──────────────────────────────────────
		// 1차 응답 수신 (검사 시작 ACK)
		void OnMeasure(ByteArray body) override;
		// 2차 응답 수신 (결과)
		void OnInspReady(ByteArray body) override;
		void OnSetCok(ByteArray body) override;
		void OnDeviceCheck(ByteArray body) override;
		void OnLight(ByteArray body) override;
		void Process() override;

	private:
		VisionPlviProcessor(const VisionPlviProcessor&) = delete;
		VisionPlviProcessor& operator=(const VisionPlviProcessor&) = delete;

		// Pocket 결과 배열 → 콤마 구분 문자열 파싱 헬퍼
		// ex) "0,99,1,2,11" 형태로 DataMap에 저장
		std::string ParsePocketResult(const CPacketBody_S107F6& pkt,
			int ctrayX, int ctrayY);
	};

} // namespace VMF
