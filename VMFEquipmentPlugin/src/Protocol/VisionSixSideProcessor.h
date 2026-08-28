#pragma once

#include "VisionMotionFramework\visioncommunicationmanager.h"
#include "PlugInAPI.h"

namespace VMF_PLUGIN
{
	// ================================================================
	// VisionSixSideProcessor
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

    class VMF_PLUGIN_API VisionSixSideProcessor : public VMF::VisionCommunicationManager
	{
	public:
		VisionSixSideProcessor();
		~VisionSixSideProcessor() override;

        // ── Request 함수 ─────────────────────────────────────────────
		// 6면 검사 요청 (CMD 1102, S107/F1)
		// params 필요 키:
		//   SixSide::VisionType, SixSide::CamPosition,
		//   SixSide::FacePosition, SixSide::SelectCount,
		//   SixSide::Skip, SixSide::BarcodeId, SixSide::LotId
		bool RequestMeasureAsync(const VMF::StringMap& params) override;

		// 미사용
		bool RequestSetCokAsync(const VMF::StringMap& params) override;
		bool RequestInspReadyAsync(const VMF::StringMap& params) override;
		bool RequestDeviceCheckAsync(const VMF::StringMap& params) override;
		bool RequestLightAsync(const VMF::StringMap& params) override;

		// ── On 함수 (수신 콜백) ──────────────────────────────────────
		// 6면 검사 결과 수신 (S107/F2)
		void OnMeasure(VMF::ByteArray body) override;
		void OnSetCok(VMF::ByteArray body) override;
		void OnInspReady(VMF::ByteArray body) override;
		void OnDeviceCheck(VMF::ByteArray body) override;
		void OnLight(VMF::ByteArray body) override;
		void Process() override;

	private:
		VisionSixSideProcessor(const VisionSixSideProcessor&) = delete;
		VisionSixSideProcessor& operator=(const VisionSixSideProcessor&) = delete;
	};

} // namespace VMF_PLUGIN
