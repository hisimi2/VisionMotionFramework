#include "pch.h"
#include "VisionSixSideProcessor.h"
#include "VisionComm\SecsMessageDispatcher.h"
#include "VisionComm\SECSPacket.h"
#include "VisionParamKeysSixSide.h"
#include "VisionPacket.h"
#include <cstring>

using namespace VMF;
using namespace VMF_PLUGIN;
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
//   nStatus    = VisionType (SixSide::VisionType)
//   cData[0]   = CamPosition (SixSide::CamPosition)
//   cData[1]   = 면 번호 (SixSide::FacePosition) "1"~"6"
//   cData[2]   = SelectCount (SixSide::SelectCount)
//   cData[3]   = Skip 여부 (SixSide::Skip) "0"=검사, "1"=Skip
//   cData[4]   = Barcode ID (SixSide::BarcodeId)
//   cData[5]   = Lot ID (SixSide::LotId)
// ================================================================
bool VisionSixSideProcessor::RequestMeasureAsync(const StringMap& params)
{
	ClearLatestData(Measure);

	CPacketBody_S107F1_6Side body;
	body.Clear();

	// nDataID = 1102 고정
	body.nDataID = 1102;

	// nStatus = VisionType
	auto it = params.find(SixSide::VisionType);
	if (it != params.end()) body.nStatus = std::stoi(it->second);

	// cData[0] = CamPosition
	it = params.find(SixSide::CamPosition);
	if (it != params.end()) body.SetData(0, it->second.c_str());

	// cData[1] = 면 번호 (1~6)
	it = params.find(SixSide::FacePosition);
	if (it != params.end()) body.SetData(1, it->second.c_str());

	// cData[2] = SelectCount
	it = params.find(SixSide::SelectCount);
	if (it != params.end()) body.SetData(2, it->second.c_str());

	// cData[3] = Skip (0=검사, 1=Skip)
	it = params.find(SixSide::Skip);
	if (it != params.end()) body.SetData(3, it->second.c_str());
	else                    body.SetData(3, "0"); // 기본값: 검사

													// cData[4] = Barcode ID
	it = params.find(SixSide::BarcodeId);
	if (it != params.end()) body.SetData(4, it->second.c_str());

	// cData[5] = Lot ID
	it = params.find(SixSide::LotId);
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
//   cData[0]   → SixSide::CamPosition  (echo)
//   cData[1]   → SixSideResult::ResultFace   (면 번호 echo)
//   cData[2]   → SixSideResult::GrabCheck    ("1"=OK, "2"=Fail)
//   cData[3]   → SixSideResult::InspResult   ("1"=OK, "2"=NG)
//   cData[4]   → SixSide::BarcodeId    (echo)
//   cData[5]   → SixSide::LotId        (echo)
//
// Task에서 사용:
//   auto& data = vp->GetLatestData(Measure);
//   data[SixSideResult::GrabCheck]  → "1"=OK, "2"=Fail
//   data[SixSideResult::InspResult] → "1"=OK, "2"=NG
//   data[SixSideResult::ResultFace] → 어느 면의 결과인지 확인
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
	data[SixSide::CamPosition] = std::string(pkt.cData[0]); // CamPosition echo
	data[SixSideResult::ResultFace] = std::string(pkt.cData[1]); // 면 번호 echo
	data[SixSideResult::GrabCheck] = std::string(pkt.cData[2]); // "1"=OK, "2"=Fail
	data[SixSideResult::InspResult] = std::string(pkt.cData[3]); // "1"=OK, "2"=NG
	data[SixSide::BarcodeId] = std::string(pkt.cData[4]); // Barcode echo
	data[SixSide::LotId] = std::string(pkt.cData[5]); // LotID echo

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


