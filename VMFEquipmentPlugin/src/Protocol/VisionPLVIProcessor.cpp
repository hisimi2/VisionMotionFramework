#include "pch.h"
#include "VisionPlviProcessor.h"
#include "SecsMessageDispatcher.h"
#include "SECSPacket.h"
#include "VisionParamKeysPLVI.h"
#include "VisionPacket.h"
#include <cstring>

using namespace VMF;
using namespace VMF_PLUGIN;
// ================================================================
// 생성자 ? 수신 핸들러 등록
// ================================================================
VisionPlviProcessor::VisionPlviProcessor()
{
	VC::SecsMessageDispatcher& disp = m_ctrl.GetDispatcher();

	// S107/F6 수신 → 1차 ACK(Measure) 또는 2차 결과(InspReady)로 분기
	// 프로토콜 상 동일 F6이므로, nDataID 또는 순서로 구분
	// 여기서는 수신 순서 기반:
	//   첫 번째 F6 → OnMeasure (검사 시작 ACK)
	//   두 번째 F6 → OnInspReady (결과)
	disp.RegisterHandler(VisionPLVIProtocol::PLVIResult,
		[this](int, int, std::vector<uint8_t>&& body, int)
	{
		// 1차 ACK 수신 전이면 OnMeasure, 이후면 OnInspReady
		if (!HasReceived(Measure))
			this->OnMeasure(std::move(body));
		else
			this->OnInspReady(std::move(body));
	});
}

VisionPlviProcessor::~VisionPlviProcessor() = default;

// ================================================================
// [1차] 검사 시작 요청 (REQ_MEASURE, S107/F5)
//
// 패킷 조립:
//   nDataID    = PLVI 요청 ID (Strategy에서 DATA_ID로 설정)
//   nStatus    = 0 (미사용)
//   cData[0]   = PLVI 위치 (PLVI::Position)
//   cData[1]   = PKG 명칭 (PLVI::PkgName)
//   cData[2]   = "CTrayX,CTrayY" 형태 문자열
//   cData[3]   = Device 유무 배열 ("0,99,99,0,..." 콤마 구분)
// ================================================================
bool VisionPlviProcessor::RequestMeasureAsync(const VMF::StringMap& params)
{
	ClearLatestData(Measure);

	CPacketBody_S107F5 body;
	body.Clear();

	// nDataID ? PLVI 요청 ID
	auto it = params.find("DATA_ID");
	if (it != params.end()) body.nDataID = std::stoi(it->second);

	// cData[0] ? PLVI 위치
	it = params.find(PLVI::Position);
	if (it != params.end()) body.SetData(0, it->second.c_str());

	// cData[1] ? PKG 명칭
	it = params.find(PLVI::PkgName);
	if (it != params.end()) body.SetData(1, it->second.c_str());

	// cData[2] ? C-Tray 크기 "X,Y" 형태
	{
		std::string ctrayX = "8", ctrayY = "4";
		auto ix = params.find(PLVI::CtrayX);
		auto iy = params.find(PLVI::CtrayY);
		if (ix != params.end()) ctrayX = ix->second;
		if (iy != params.end()) ctrayY = iy->second;
		std::string ctrayStr = ctrayX + "," + ctrayY;
		body.SetData(2, ctrayStr.c_str());
	}

	// cData[3] ? Device 유무 배열 ("0,99,99,0,..." 콤마 구분)
	it = params.find(PLVI::DeviceInfo);
	if (it != params.end()) body.SetData(3, it->second.c_str());

	// 패킷 송신
	std::vector<uint8_t> bodyBytes;
	const uint8_t* p = reinterpret_cast<const uint8_t*>(&body);
	bodyBytes.assign(p, p + sizeof(body));

	VC::SECSPacket secsPkt;
	secsPkt.SetCorrelationId(body.nDataID);
	secsPkt.SetProtocol(VisionPLVIProtocol::PLVIRequest);
	secsPkt.SetBody(bodyBytes);

	return (m_ctrl.SendPacketAsync(secsPkt) == VC::VisionOK);
}

// ================================================================
// [2차] 결과 요청 (REQ_MEASURE, S107/F5)
//
// 패킷 조립:
//   nDataID    = PLVI 요청 ID
//   cData      = 미사용
// ================================================================
bool VisionPlviProcessor::RequestInspReadyAsync(const StringMap& params)
{
	ClearLatestData(InspReady);

	CPacketBody_S107F5 body;
	body.Clear();

	auto it = params.find("DATA_ID");
	if (it != params.end()) body.nDataID = std::stoi(it->second);

	std::vector<uint8_t> bodyBytes;
	const uint8_t* p = reinterpret_cast<const uint8_t*>(&body);
	bodyBytes.assign(p, p + sizeof(body));

	VC::SECSPacket secsPkt;
	secsPkt.SetCorrelationId(body.nDataID);
	secsPkt.SetProtocol(VisionPLVIProtocol::PLVIRequest);
	secsPkt.SetBody(bodyBytes);

	return (m_ctrl.SendPacketAsync(secsPkt) == VC::VisionOK);
}

// ================================================================
// [OnMeasure] 1차 응답 수신 ? 검사 시작 ACK (S107/F6)
//
// 파싱:
//   nStatus    → PLVIResult::Status  ("0"=ERROR, "1"=SUCCESS)
//   cData[0]   → PLVIResult::ErrCode
//
// Task에서 사용:
//   auto& data = vp->GetLatestData(Measure);
//   data[PLVIResult::Status]   → "1"이면 검사 시작 OK
//   data[PLVIResult::ErrCode] → 에러 코드 확인
// ================================================================
void VisionPlviProcessor::OnMeasure(ByteArray body)
{
	if (body.size() < sizeof(CPacketBody_S107F6))
	{
		ClearLatestData(Measure); return;
	}
	CPacketBody_S107F6 pkt;
	std::memcpy(&pkt, body.data(), sizeof(pkt));

	DataMap data;
	data[PLVIResult::Status] = std::to_string(pkt.nStatus); // 0=ERROR, 1=SUCCESS
	data[PLVIResult::ErrCode] = std::string(pkt.cData[0]);   // 에러 코드

	SetLatestData(Measure, data);
	SetReceived(Measure, true);
}

// ================================================================
// [OnInspReady] 2차 응답 수신 ? PLVI 검사 결과 (S107/F6)
//
// 파싱:
//   nStatus    → PLVIResult::Status         ("0"=ERROR, "1"=SUCCESS)
//   cData[0]   → PLVIResult::ErrCode       (에러 코드)
//   cData[1]   → PLVIResult::OverallResult ("0"=OK, "1"=NG)
//   cData[2]   → PLVIResult::ResultPosition (PLVI 위치 echo)
//   cData[3]   → PLVIResult::PocketResult  (개별 Pocket 상태 콤마 구분)
//
// Task에서 사용:
//   auto& data = vp->GetLatestData(InspReady);
//   data[PLVIResult::Status]          → "1"이면 수신 성공
//   data[PLVIResult::OverallResult]  → "0"=OK, "1"=NG
//   data[PLVIResult::PocketResult]   → "0,99,1,2,11,..." 파싱
// ================================================================
void VisionPlviProcessor::OnInspReady(ByteArray body)
{
	if (body.size() < sizeof(CPacketBody_S107F6))
	{
		ClearLatestData(InspReady); return;
	}
	CPacketBody_S107F6 pkt;
	std::memcpy(&pkt, body.data(), sizeof(pkt));

	DataMap data;
	data[PLVIResult::Status] = std::to_string(pkt.nStatus);
	data[PLVIResult::ErrCode] = std::string(pkt.cData[0]);
	data[PLVIResult::OverallResult] = std::string(pkt.cData[1]); // "0"=OK, "1"=NG
	data[PLVIResult::ResultPosition] = std::string(pkt.cData[2]);
	// cData[3]은 이미 "0,99,1,2,11,..." 형태로 콤마 구분되어 있음
	data[PLVIResult::PocketResult] = std::string(pkt.cData[3]);

	SetLatestData(InspReady, data);
	SetReceived(InspReady, true);
}

bool VisionPlviProcessor::RequestSetCokAsync(const StringMap&) { return false; }
bool VisionPlviProcessor::RequestDeviceCheckAsync(const StringMap&) { return false; }
bool VisionPlviProcessor::RequestLightAsync(const StringMap&) { return false; }
void VisionPlviProcessor::OnSetCok(ByteArray) {}
void VisionPlviProcessor::OnDeviceCheck(ByteArray) {}
void VisionPlviProcessor::OnLight(ByteArray) {}
void VisionPlviProcessor::Process() { VisionCommunicationManager::Process(); }

// ================================================================
// [ParsePocketResult] Pocket 결과 배열 파싱 헬퍼
// ================================================================
std::string VisionPlviProcessor::ParsePocketResult(const CPacketBody_S107F6& pkt,
    int ctrayX, int ctrayY)
{
    // pkt.cData[3]은 이미 "0,99,1,2,11,12,..." 형태의 콤마 구분 문자열
    // 그대로 반환 (Task에서 필요시 파싱)
    return std::string(pkt.cData[3]);
}
