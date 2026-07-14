#include "pch.h"
#include "VisionMemoryProcessor.h"
#include "SecsMessageDispatcher.h"
#include "SECSPacket.h"
#include "scr\Protocol\VisionMemoryKeys.h"
#include "VisionPacketMemory.h"

#include <vector>
#include <cstring>

namespace VMF
{
    VisionMemoryProcessor::VisionMemoryProcessor()
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

    VisionMemoryProcessor::~VisionMemoryProcessor() = default;

    // -----------------------------------------------------------------------
    // [RequestSetCokAsync] - SetCok 요청
    // -----------------------------------------------------------------------
    bool VisionMemoryProcessor::RequestSetCokAsync(const StringMap& params)
    {
        ClearLatestData(SetCok);

        CPacketBody_S2F41 body;
        body.nCmd = 1000;
        body.nParamCount = 7;

        auto it = params.find(RECIPE_NAME);
        if (it != params.end()) strncpy_s(body.szParam[1], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(PCD_MODE);
        if (it != params.end()) strncpy_s(body.szParam[2], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(DEVICE_SIZE_X);
        if (it != params.end()) strncpy_s(body.szParam[3], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(DEVICE_SIZE_Y);
        if (it != params.end()) strncpy_s(body.szParam[4], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(COK_TYPE);
        if (it != params.end()) strncpy_s(body.szParam[5], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(PICKER_PITCH_X);
        if (it != params.end()) strncpy_s(body.szParam[6], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(PICKER_PITCH_Y);
        if (it != params.end()) strncpy_s(body.szParam[7], STR_LEN, it->second.c_str(), _TRUNCATE);

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
    bool VisionMemoryProcessor::RequestInspReadyAsync(const StringMap& params)
    {
        // InspReady는 SetCok와 동일한 ControlRequest 패킷 사용
        return RequestSetCokAsync(params);
    }

    // -----------------------------------------------------------------------
    // [RequestMeasureAsync] - Measure 요청
    // -----------------------------------------------------------------------
    bool VisionMemoryProcessor::RequestMeasureAsync(const StringMap& params)
    {
        ClearLatestData(Measure);

        CPacketBody_S107F9 body;

        auto it = params.find(CAMERA_ID);
        if (it != params.end()) body.nDataID = std::stoi(it->second);
        it = params.find(INSPECTION_TYPE);
        if (it != params.end()) body.nStatus = std::stoi(it->second);
        it = params.find(MOVE_PART);
        if (it != params.end()) body.SetData(0, it->second.c_str());
        it = params.find(SAVE_IMAGE);
        if (it != params.end()) body.SetData(1, it->second.c_str());
        it = params.find(FOV_DIRECTION);
        if (it != params.end()) body.SetData(3, it->second.c_str());

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
    bool VisionMemoryProcessor::RequestDeviceCheckAsync(const StringMap& params)
    {
        (void)params;
        return false;
    }

    // -----------------------------------------------------------------------
    // [RequestLightAsync] - Light 요청
    // -----------------------------------------------------------------------
    bool VisionMemoryProcessor::RequestLightAsync(const StringMap& params)
    {
        (void)params;
        return false;
    }

    // -----------------------------------------------------------------------
    // [OnSetCok] - SetCok 응답 처리
    // -----------------------------------------------------------------------
    void VisionMemoryProcessor::OnSetCok(ByteArray body)
    {
        if (body.size() < sizeof(CPacketBody_S2F41))
        {
            ClearLatestData(SetCok); return;
        }

        CPacketBody_S2F41 pkt;
        std::memcpy(&pkt, body.data(), sizeof(pkt));

        DataMap data;
        data[RESULT] = std::string(pkt.szParam[0]);
        data[SERVER_INDEX] = std::string(pkt.szParam[1]);
        data[CAM_STATUS] = std::string(pkt.szParam[2]);
        data[CAM_TYPE] = std::string(pkt.szParam[3]);
        SetLatestData(SetCok, data);
    }

    // -----------------------------------------------------------------------
    // [OnInspReady] - InspReady 응답 처리
    // -----------------------------------------------------------------------
    void VisionMemoryProcessor::OnInspReady(ByteArray body)
    {
        if (body.size() < sizeof(CPacketBody_S2F41))
        {
            ClearLatestData(InspReady); return;
        }

        CPacketBody_S2F41 pkt;
        std::memcpy(&pkt, body.data(), sizeof(pkt));

        DataMap data;
        data[RESULT] = std::string(pkt.szParam[0]);
        data[SERVER_INDEX] = std::string(pkt.szParam[1]);
        data[CAM_STATUS] = std::string(pkt.szParam[2]);
        data[CAM_TYPE] = std::string(pkt.szParam[3]);
        SetLatestData(InspReady, data);
    }

    // -----------------------------------------------------------------------
    // [OnMeasure] - Measure 응답 처리
    // -----------------------------------------------------------------------
    void VisionMemoryProcessor::OnMeasure(ByteArray body)
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
        data[Z_FOCUS_VALUE] = pkt.cData[0];
        data[X_OFFSET] = pkt.cData[1];
        data[Y_OFFSET] = pkt.cData[2];
        data[ANGLE] = pkt.cData[3];
        data[AUTO_VISION_SETTING] = pkt.cData[4];
        SetLatestData(Measure, data);
    }

    // -----------------------------------------------------------------------
    // [OnDeviceCheck] - DeviceCheck 응답 처리
    // -----------------------------------------------------------------------
    void VisionMemoryProcessor::OnDeviceCheck(ByteArray body)
    {
        (void)body;
        ClearLatestData(DeviceCheck);
    }

    // -----------------------------------------------------------------------
    // [OnLight] - Light 응답 처리
    // -----------------------------------------------------------------------
    void VisionMemoryProcessor::OnLight(ByteArray body)
    {
        (void)body;
        ClearLatestData(Light);
    }

    // -----------------------------------------------------------------------
    // [Process] - 기본 Process 호출
    // -----------------------------------------------------------------------
    void VisionMemoryProcessor::Process()
    {
        VisionProcessorBase::Process();
    }

    std::vector<std::string> VisionMemoryProcessor::ParseMeasureBody(
        const ByteArray& body)
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

} // namespace VMF
