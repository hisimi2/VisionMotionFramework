#include "pch.h"
#include "VisionSixSideProcessor.h"
#include "SecsMessageDispatcher.h"
#include "SECSPacket.h"
#include <cstring>

namespace VMF
{
	// ================================================================
	// 생성자 ? 수신 핸들러 등록
	// ================================================================
	VisionSixSideProcessor::VisionSixSideProcessor()
	{
		VC::SecsMessageDispatcher& disp = m_ctrl.GetDispatcher();

		// S107/F2 수신 → OnMeasure 호출
		disp.RegisterHandler(Vision6SideProtocol::SideResult,
			[this](int, int, std::vector<uint8_t>&& body, int)
		{
			this->OnMeasure(std::move(body));
		});
	}

	VisionSixSideProcessor::~VisionSixSideProcessor() = default;

	// ================================================================
	// [RequestMeasureAsync] 6면 검사 요청 (CMD 1102, S107/F1)
	//
	// 패킷 조립:
	//   nDataID    = 1102 (CMD ID 고정)
	//   nStatus    = VisionType (SIDE6_VISION_TYPE)
	//   cData[0]   = CamPosition (SIDE6_CAM_POSITION)
	//   cData[1]   = 면 번호 (SIDE6_FACE_POSITION) "1"~"6"
	//   cData[2]   = SelectCount (SIDE6_SELECT_COUNT)
	//   cData[3]   = Skip 여부 (SIDE6_SKIP) "0"=검사, "1"=Skip
	//   cData[4]   = Barcode ID (SIDE6_BARCODE_ID)
	//   cData[5]   = Lot ID (SIDE6_LOT_ID)
	// ================================================================
	bool VisionSixSideProcessor::RequestMeasureAsync(const StringMap& params)
	{
		ClearLatestData(Measure);

		CPacketBody_S107F1_6Side body;
		body.Clear();

		// nDataID = 1102 고정
		body.nDataID = 1102;

		// nStatus = VisionType
		auto it = params.find(SIDE6_VISION_TYPE);
		if (it != params.end()) body.nStatus = std::stoi(it->second);

		// cData[0] = CamPosition
		it = params.find(SIDE6_CAM_POSITION);
		if (it != params.end()) body.SetData(0, it->second.c_str());

		// cData[1] = 면 번호 (1~6)
		it = params.find(SIDE6_FACE_POSITION);
		if (it != params.end()) body.SetData(1, it->second.c_str());

		// cData[2] = SelectCount
		it = params.find(SIDE6_SELECT_COUNT);
		if (it != params.end()) body.SetData(2, it->second.c_str());

		// cData[3] = Skip (0=검사, 1=Skip)
		it = params.find(SIDE6_SKIP);
		if (it != params.end()) body.SetData(3, it->second.c_str());
		else                    body.SetData(3, "0"); // 기본값: 검사

													  // cData[4] = Barcode ID
		it = params.find(SIDE6_BARCODE_ID);
		if (it != params.end()) body.SetData(4, it->second.c_str());

		// cData[5] = Lot ID
		it = params.find(SIDE6_LOT_ID);
		if (it != params.end()) body.SetData(5, it->second.c_str());

		// 패킷 송신
		std::vector<uint8_t> bodyBytes;
		const uint8_t* p = reinterpret_cast<const uint8_t*>(&body);
		bodyBytes.assign(p, p + sizeof(body));

		VC::SECSPacket secsPkt;
		secsPkt.SetCorrelationId(body.nDataID);
		secsPkt.SetProtocol(Vision6SideProtocol::SideRequest);
		secsPkt.SetBody(bodyBytes);

		return (m_ctrl.SendPacketAsync(secsPkt) == VC::VisionOK);
	}

	// ================================================================
	// [OnMeasure] 6면 검사 결과 수신 (S107/F2)
	//
	// 파싱:
	//   nDataID    = 1102 (echo)
	//   nStatus    = VisionType (echo)
	//   cData[0]   → SIDE6_CAM_POSITION  (echo)
	//   cData[1]   → SIDE6_RESULT_FACE   (면 번호 echo)
	//   cData[2]   → SIDE6_GRAB_CHECK    ("1"=OK, "2"=Fail)
	//   cData[3]   → SIDE6_INSP_RESULT   ("1"=OK, "2"=NG)
	//   cData[4]   → SIDE6_BARCODE_ID    (echo)
	//   cData[5]   → SIDE6_LOT_ID        (echo)
	//
	// Task에서 사용:
	//   auto& data = vp->GetLatestData(Measure);
	//   data[SIDE6_GRAB_CHECK]  → "1"=OK, "2"=Fail
	//   data[SIDE6_INSP_RESULT] → "1"=OK, "2"=NG
	//   data[SIDE6_RESULT_FACE] → 어느 면의 결과인지 확인
	// ================================================================
	void VisionSixSideProcessor::OnMeasure(ByteArray body)
	{
		if (body.size() < sizeof(CPacketBody_S107F2_6Side))
		{
			ClearLatestData(Measure); return;
		}
		CPacketBody_S107F2_6Side pkt;
		std::memcpy(&pkt, body.data(), sizeof(pkt));

		DataMap data;
		data[SIDE6_CAM_POSITION] = std::string(pkt.cData[0]); // CamPosition echo
		data[SIDE6_RESULT_FACE] = std::string(pkt.cData[1]); // 면 번호 echo
		data[SIDE6_GRAB_CHECK] = std::string(pkt.cData[2]); // "1"=OK, "2"=Fail
		data[SIDE6_INSP_RESULT] = std::string(pkt.cData[3]); // "1"=OK, "2"=NG
		data[SIDE6_BARCODE_ID] = std::string(pkt.cData[4]); // Barcode echo
		data[SIDE6_LOT_ID] = std::string(pkt.cData[5]); // LotID echo

		SetLatestData(Measure, data);
		SetReceived(Measure, true);
	}

	bool VisionSixSideProcessor::RequestSetCokAsync(const StringMap&) { return false; }
	bool VisionSixSideProcessor::RequestInspReadyAsync(const StringMap&) { return false; }
	bool VisionSixSideProcessor::RequestDeviceCheckAsync(const StringMap&) { return false; }
	bool VisionSixSideProcessor::RequestLightAsync(const StringMap&) { return false; }
	void VisionSixSideProcessor::OnSetCok(ByteArray) {}
	void VisionSixSideProcessor::OnInspReady(ByteArray) {}
	void VisionSixSideProcessor::OnDeviceCheck(ByteArray) {}
	void VisionSixSideProcessor::OnLight(ByteArray) {}
	void VisionSixSideProcessor::Process() { VisionCommunicationManager::Process(); }

} // namespace VMF
