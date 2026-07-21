#pragma once
#include "VisionCommunicationManager.h"
#include "scr\Protocol\VisionMemoryKeys.h"
#include "VisionPacketMemory.h"
#include "VMFEquipmentPluginExport.h"

namespace VMF
{
	// ================================================================
	// Vision6SideProcessor
	//
	// 6면 검사 전용 VisionProcessor.
	// S107/F1 (CMD_6_SIDE_INSPECTION_MEASURE) 로 면별 검사 요청.
	// S107/F2 로 결과 수신.
	//
	// VisionCommand 매핑:
	//   Measure → 6면 검사 요청 (면 번호만 바꿔 6회 반복)
	// ================================================================
	namespace Vision6SideProtocol
	{
		enum class Stream : int { Measure = 107 };
		enum class Function : int
		{
			Request = 1,  // H→V
			Result = 2   // V→H
		};
		static const VC::VisionProtocolId SideRequest(
			static_cast<int>(Stream::Measure),
			static_cast<int>(Function::Request));
		static const VC::VisionProtocolId SideResult(
			static_cast<int>(Stream::Measure),
			static_cast<int>(Function::Result));
	}

    class VMF_PLUGIN_API Vision6SideProcessor : public VisionCommunicationManager
	{
	public:
		Vision6SideProcessor();
		~Vision6SideProcessor() override;

		// ── Request 함수 ─────────────────────────────────────────────
		// 6면 검사 요청 (CMD 1102, S107/F1)
		// params 필요 키:
		//   SIDE6_VISION_TYPE, SIDE6_CAM_POSITION,
		//   SIDE6_FACE_POSITION, SIDE6_SELECT_COUNT,
		//   SIDE6_SKIP, SIDE6_BARCODE_ID, SIDE6_LOT_ID
		bool RequestMeasureAsync(const StringMap& params) override;

		// 미사용
		bool RequestSetCokAsync(const StringMap& params) override;
		bool RequestInspReadyAsync(const StringMap& params) override;
		bool RequestDeviceCheckAsync(const StringMap& params) override;
		bool RequestLightAsync(const StringMap& params) override;

		// ── On 함수 (수신 콜백) ──────────────────────────────────────
		// 6면 검사 결과 수신 (S107/F2)
		void OnMeasure(ByteArray body) override;
		void OnSetCok(ByteArray body) override;
		void OnInspReady(ByteArray body) override;
		void OnDeviceCheck(ByteArray body) override;
		void OnLight(ByteArray body) override;
		void Process() override;

	private:
		Vision6SideProcessor(const Vision6SideProcessor&) = delete;
		Vision6SideProcessor& operator=(const Vision6SideProcessor&) = delete;
	};

} // namespace VMF
