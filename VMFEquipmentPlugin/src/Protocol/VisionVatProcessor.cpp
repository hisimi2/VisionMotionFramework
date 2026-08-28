#include "pch.h"
#include "VisionVatProcessor.h"
#include "VisionComm\SecsMessageDispatcher.h"
#include "VisionComm\SECSPacket.h"
#include "VisionParamKeysVAT.h"
#include "VisionPacket.h"

#include <vector>
#include <cstring>

using namespace VMF;
using namespace VMF_PLUGIN;

VisionVatProcessor::VisionVatProcessor()
{
    VC::SecsMessageDispatcher& disp = m_ctrl.GetDispatcher();

    disp.RegisterHandler(VisionMemoryProtocol::Measure,
        [this](int, int, std::vector<uint8_t>&& body, int)
        {
            this->OnMeasure(std::move(body));
        });

    disp.RegisterHandler(VisionMemoryProtocol::ControlAck,
        [this](int, int, std::vector<uint8_t>&& body, int)
        {
            auto copy = body;
            this->OnSetCok(std::move(body));
            this->OnInspReady(std::move(copy));
        });
}

VisionVatProcessor::~VisionVatProcessor() = default;

// -----------------------------------------------------------------------
// [RequestSetCokAsync] - SetCok 요청
// -----------------------------------------------------------------------
bool VisionVatProcessor::RequestSetCokAsync(const StringMap& params)
{
    ClearLatestData(SetCok);

    CPacketBody_S2F41 body;
    body.nCmd = 1000;
    body.nParamCount = 7;

    // Set parameters using helper function
    SetPacketParam(body, params, VAT::RecipeName, 1);
    SetPacketParam(body, params, VAT::PcdMode, 2);
    SetPacketParam(body, params, VAT::DeviceSizeX, 3);
    SetPacketParam(body, params, VAT::DeviceSizeY, 4);
    SetPacketParam(body, params, VAT::CokType, 5);
    SetPacketParam(body, params, VAT::PickerPitchX, 6);
    SetPacketParam(body, params, VAT::PickerPitchY, 7);

    std::vector<uint8_t> bodyBytes;
    const uint8_t* p = reinterpret_cast<const uint8_t*>(&body);
    bodyBytes.assign(p, p + sizeof(body));

    VC::SECSPacket secsPkt;
    secsPkt.SetCorrelationId(body.nCmd);
    secsPkt.SetProtocol(VisionMemoryProtocol::ControlRequest);
    secsPkt.SetBody(bodyBytes);

    return (m_ctrl.SendPacketAsync(secsPkt) == VC::VisionOK);
}

// -----------------------------------------------------------------------
// [RequestInspReadyAsync] - InspReady 요청
// -----------------------------------------------------------------------
bool VisionVatProcessor::RequestInspReadyAsync(const StringMap& params)
{
    // InspReady는 SetCok와 동일한 ControlRequest 패킷 사용
    return RequestSetCokAsync(params);
}

// -----------------------------------------------------------------------
// [RequestMeasureAsync] - Measure 요청
// -----------------------------------------------------------------------
bool VisionVatProcessor::RequestMeasureAsync(const StringMap& params)
{
    ClearLatestData(Measure);

    CPacketBody_S107F9 body;

    // Set parameters using helper function - Use CameraIndex (what Builders actually use)
    SetPacketParam(body, params, VAT::CameraIndex, "nDataID");
    SetPacketParam(body, params, VAT::InspectionType, "nStatus");
    SetPacketParam(body, params, VAT::MovePart, 0);
    SetPacketParam(body, params, VAT::SaveImage, 1);
    SetPacketParam(body, params, VAT::FovDirection, 3);

    std::vector<uint8_t> bodyBytes;
    const uint8_t* p = reinterpret_cast<const uint8_t*>(&body);
    bodyBytes.assign(p, p + sizeof(body));

    VC::SECSPacket secsPkt;
    secsPkt.SetCorrelationId(body.nDataID);
    secsPkt.SetProtocol(VisionMemoryProtocol::Measure);
    secsPkt.SetBody(bodyBytes);

    return (m_ctrl.SendPacketAsync(secsPkt) == VC::VisionOK);
}

// -----------------------------------------------------------------------
// [RequestDeviceCheckAsync] - DeviceCheck 요청
// -----------------------------------------------------------------------
bool VisionVatProcessor::RequestDeviceCheckAsync(const StringMap& params)
{
    (void)params;
    return false;
}

// -----------------------------------------------------------------------
// [RequestLightAsync] - Light 요청
// -----------------------------------------------------------------------
bool VisionVatProcessor::RequestLightAsync(const StringMap& params)
{
    (void)params;
    return false;
}

// -----------------------------------------------------------------------
// Helper: Set packet parameter (non-template implementations)
// -----------------------------------------------------------------------
void VisionVatProcessor::SetPacketParam(CPacketBody_S2F41& body, const StringMap& params, const char* key, int index)
{
    auto it = params.find(key);
    if (it != params.end())
    {
        std::string value = it->second;
        if (index >= 0 && static_cast<size_t>(index) < DATA_SIZE)
        {
            strncpy_s(body.szParam[index], STR_LEN, value.c_str(), _TRUNCATE);
        }
    }
}

void VisionVatProcessor::SetPacketParam(CPacketBody_S107F9& body, const StringMap& params, const char* key, const char* fieldName)
{
    auto it = params.find(key);
    if (it != params.end())
    {
        std::string value = it->second;
        if (strcmp(fieldName, "nDataID") == 0)
        {
            body.nDataID = std::stoi(value);
        }
        else if (strcmp(fieldName, "nStatus") == 0)
        {
            body.nStatus = std::stoi(value);
        }
    }
}

void VisionVatProcessor::SetPacketParam(CPacketBody_S107F9& body, const StringMap& params, const char* key, int index)
{
    auto it = params.find(key);
    if (it != params.end())
    {
        std::string value = it->second;
        if (index >= 0 && static_cast<size_t>(index) < DATA_SIZE)
        {
            strncpy_s(body.cData[index], STR_LEN, value.c_str(), _TRUNCATE);
        }
    }
}

// -----------------------------------------------------------------------
// [OnSetCok] - SetCok 응답 처리
// -----------------------------------------------------------------------
void VisionVatProcessor::OnSetCok(ByteArray body)
{
    if (body.size() < sizeof(CPacketBody_S2F41))
    {
        ClearLatestData(SetCok); return;
    }

    CPacketBody_S2F41 pkt;
    std::memcpy(&pkt, body.data(), sizeof(pkt));

    DataMap data;
    data[VATResult::Result] = std::string(pkt.szParam[0]);
    data[VATResult::ServerIndex] = std::string(pkt.szParam[1]);
    data[VATResult::CamStatus] = std::string(pkt.szParam[2]);
    data[VATResult::CamType] = std::string(pkt.szParam[3]);
    SetLatestData(SetCok, data);
}

// -----------------------------------------------------------------------
// [OnInspReady] - InspReady 응답 처리
// -----------------------------------------------------------------------
void VisionVatProcessor::OnInspReady(ByteArray body)
{
    if (body.size() < sizeof(CPacketBody_S2F41))
    {
        ClearLatestData(InspReady); return;
    }

    CPacketBody_S2F41 pkt;
    std::memcpy(&pkt, body.data(), sizeof(pkt));

    DataMap data;
    data[VATResult::Result] = std::string(pkt.szParam[0]);
    data[VATResult::ServerIndex] = std::string(pkt.szParam[1]);
    data[VATResult::CamStatus] = std::string(pkt.szParam[2]);
    data[VATResult::CamType] = std::string(pkt.szParam[3]);
    SetLatestData(InspReady, data);
}

// -----------------------------------------------------------------------
// [OnMeasure] - Measure 응답 처리
// -----------------------------------------------------------------------
void VisionVatProcessor::OnMeasure(ByteArray body)
{
    if (body.size() < sizeof(CPacketBody_S107F9))
    {
        ClearLatestData(Measure); return;
    }

    CPacketBody_S107F9 pkt;
    std::memcpy(&pkt, body.data(), sizeof(pkt));

    if (pkt.nStatus != 1)
    {
        ClearLatestData(Measure); return;
    }

    DataMap data;
    data[VATResult::ZFocusValue] = pkt.cData[0];
    data[VATResult::XOffset] = pkt.cData[1];
    data[VATResult::YOffset] = pkt.cData[2];
    data[VATResult::Angle] = pkt.cData[3];
    data[VATResult::AutoVisionSetting] = pkt.cData[4];
    SetLatestData(Measure, data);
}

// -----------------------------------------------------------------------
// [OnDeviceCheck] - DeviceCheck 응답 처리
// -----------------------------------------------------------------------
void VisionVatProcessor::OnDeviceCheck(ByteArray body)
{
    (void)body;
    ClearLatestData(DeviceCheck);
}

// -----------------------------------------------------------------------
// [OnLight] - Light 응답 처리
// -----------------------------------------------------------------------
void VisionVatProcessor::OnLight(ByteArray body)
{
    (void)body;
    ClearLatestData(Light);
}

// -----------------------------------------------------------------------
// [Process] - 기본 Process 호출
// -----------------------------------------------------------------------
void VisionVatProcessor::Process()
{
    VisionCommunicationManager::Process();
}

std::vector<std::string> VisionVatProcessor::ParseMeasureBody(
    const VMF::ByteArray& body)
{
    std::vector<std::string> results;
    if (body.size() < sizeof(CPacketBody_S107F9)) return results;

    CPacketBody_S107F9 pkt;
    std::memcpy(&pkt, body.data(), sizeof(pkt));
    if (pkt.nStatus != 1) return results;

    results.push_back(pkt.cData[0]);
    results.push_back(pkt.cData[1]);
    results.push_back(pkt.cData[2]);
    results.push_back(pkt.cData[3]);
    results.push_back(pkt.cData[4]);
    return results;
}
