#include "pch.h"
#include "SecsMessageDispatcher.h"
#include "SECSPacket.h"

#include "include/VisionProcessor.h"
#include "include/VisionParamKeys.h"
#include "include/VisionPacket.h"

#include <vector>
#include <cstring>

namespace VMF
{
    VisionProcessor::VisionProcessor()
    {
        VC::SecsMessageDispatcher& disp = m_ctrl.GetDispatcher();

        disp.RegisterHandler(Protocol_Measure,
            [this](int, int, std::vector<uint8_t>&& body, int)
            {
                OnMeasure(std::move(body));
            });

        disp.RegisterHandler(Protocol_ControlAck,
            [this](int, int, std::vector<uint8_t>&& body, int)
            {
                auto copy = body;
                OnSetCok(std::move(body));
                OnInspReady(std::move(copy));
            });
    }

    VisionProcessor::~VisionProcessor() = default;

    // -----------------------------------------------------------------------
    // 개별 RequestAsync 메서드들
    // -----------------------------------------------------------------------

    bool VisionProcessor::RequestSetCokAsync(const StringMap& params)
    {
        ClearLatestData(SetCok);
        return SendControlRequest(params, SetCok);
    }

    bool VisionProcessor::RequestInspReadyAsync(const StringMap& params)
    {
        ClearLatestData(InspReady);
        return SendControlRequest(params, InspReady);
    }

    bool VisionProcessor::RequestMeasureAsync(const StringMap& params)
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
        secsPkt.SetProtocol(Protocol_Measure);
        secsPkt.SetBody(bodyBytes);

        return (m_ctrl.SendPacketAsync(secsPkt) == VC::VisionOK);
    }

    bool VisionProcessor::RequestDeviceCheckAsync(const StringMap& /*params*/)
    {
        return false;
    }

    bool VisionProcessor::RequestLightAsync(const StringMap& /*params*/)
    {
        return false;
    }

    // -----------------------------------------------------------------------
    // 공통 Control 요청 전송 헬퍼
    // -----------------------------------------------------------------------
    bool VisionProcessor::SendControlRequest(const StringMap& params, VisionCommand /*cmd*/)
    {
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
        secsPkt.SetProtocol(Protocol_ControlRequest);
        secsPkt.SetBody(bodyBytes);

        return (m_ctrl.SendPacketAsync(secsPkt) == VC::VisionOK);
    }

    // -----------------------------------------------------------------------
    // 개별 OnVisionResponse 메서드들
    // -----------------------------------------------------------------------

    void VisionProcessor::OnSetCok(ByteArray body)
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

    void VisionProcessor::OnInspReady(ByteArray body)
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

    void VisionProcessor::OnMeasure(ByteArray body)
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

    void VisionProcessor::OnDeviceCheck(ByteArray /*body*/)
    {
        ClearLatestData(DeviceCheck);
    }

    void VisionProcessor::OnLight(ByteArray /*body*/)
    {
        ClearLatestData(Light);
    }

    // -----------------------------------------------------------------------
    // [Process] — 기본 Process 호출
    // -----------------------------------------------------------------------
    void VisionProcessor::Process()
    {
        VisionProcessorBase::Process();
    }

    std::vector<std::string> VisionProcessor::ParseMeasureBody(
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
