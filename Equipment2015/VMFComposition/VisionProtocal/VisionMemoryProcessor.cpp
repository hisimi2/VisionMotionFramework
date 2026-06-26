#include "stdafx.h"
#include "VisionMemoryProcessor.h"
#include "SecsMessageDispatcher.h"
#include "SECSPacket.h"
#include "VMFComposition/VisionProtocal/VisionMemoryKeys.h"
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
                this->OnVisionResponse(VMF::VisionCommands::Measure, std::move(body));
            });

        disp.RegisterHandler(VisionMemoryProtocol::ControlAck,
            [this](int, int, std::vector<uint8_t>&& body, int)
            {
                // ControlAck는 SetCok / InspReady 두 명령에 대응
                auto copy = body;
                this->OnVisionResponse(VMF::VisionCommands::SetCok, std::move(body));
                this->OnVisionResponse(VMF::VisionCommands::InspReady, std::move(copy));
            });
    }

    VisionMemoryProcessor::~VisionMemoryProcessor() = default;

    // -----------------------------------------------------------------------
    // [RequestAsync] — cmd로 분기, 신규 기능은 여기에만 추가
    // -----------------------------------------------------------------------
    bool VisionMemoryProcessor::RequestAsync(
        VisionCommand cmd, const StringMap& params)
    {
        ClearLatestData(cmd);

        if (cmd == VMF::VisionCommands::SetCok || cmd == VMF::VisionCommands::InspReady)
            return RequestControlAsync(params);
        if (cmd == VMF::VisionCommands::Measure)
            return RequestMeasureAsyncInternal(params);
        if (cmd == VMF::VisionCommands::DeviceCheck)
            return RequestDeviceCheckAsyncInternal(params);

        return false;
    }

    // -----------------------------------------------------------------------
    // [OnVisionResponse] — cmd로 분기, 신규 기능은 여기에만 추가
    // -----------------------------------------------------------------------
    void VisionMemoryProcessor::OnVisionResponse(
        VisionCommand cmd, ByteArray body)
    {
        if (cmd == VMF::VisionCommands::SetCok)     HandleSetCok(std::move(body));
        else if (cmd == VMF::VisionCommands::InspReady)  HandleInspReady(std::move(body));
        else if (cmd == VMF::VisionCommands::Measure)    HandleMeasure(std::move(body));
        else if (cmd == VMF::VisionCommands::DeviceCheck)HandleDeviceCheck(std::move(body));
        else                        ClearLatestData(cmd);
    }

    // -----------------------------------------------------------------------
    // [내부 요청 함수]
    // -----------------------------------------------------------------------
    bool VisionMemoryProcessor::RequestControlAsync(const StringMap& params)
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
        secsPkt.SetProtocol(VisionMemoryProtocol::ControlRequest);
        secsPkt.SetBody(bodyBytes);

        return (m_ctrl.SendPacketAsync(secsPkt) == VC::VisionOK);
    }

    bool VisionMemoryProcessor::RequestMeasureAsyncInternal(
        const StringMap& params)
    {
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

    bool VisionMemoryProcessor::RequestDeviceCheckAsyncInternal(
        const StringMap& params)
    {
        (void)params;
        return false;
    }

    // -----------------------------------------------------------------------
    // [내부 수신 핸들러]
    // -----------------------------------------------------------------------
    void VisionMemoryProcessor::HandleSetCok(ByteArray body)
    {
        if (body.size() < sizeof(CPacketBody_S2F41))
        {
            ClearLatestData(VMF::VisionCommands::SetCok); return;
        }

        CPacketBody_S2F41 pkt;
        std::memcpy(&pkt, body.data(), sizeof(pkt));

        DataMap data;
        data[RESULT] = std::string(pkt.szParam[0]);
        data[SERVER_INDEX] = std::string(pkt.szParam[1]);
        data[CAM_STATUS] = std::string(pkt.szParam[2]);
        data[CAM_TYPE] = std::string(pkt.szParam[3]);
        SetLatestData(VMF::VisionCommands::SetCok, data);
    }

    void VisionMemoryProcessor::HandleInspReady(ByteArray body)
    {
        if (body.size() < sizeof(CPacketBody_S2F41))
        {
            ClearLatestData(VMF::VisionCommands::InspReady); return;
        }

        CPacketBody_S2F41 pkt;
        std::memcpy(&pkt, body.data(), sizeof(pkt));

        DataMap data;
        data[RESULT] = std::string(pkt.szParam[0]);
        data[SERVER_INDEX] = std::string(pkt.szParam[1]);
        data[CAM_STATUS] = std::string(pkt.szParam[2]);
        data[CAM_TYPE] = std::string(pkt.szParam[3]);
        SetLatestData(VMF::VisionCommands::InspReady, data);
    }

    void VisionMemoryProcessor::HandleMeasure(ByteArray body)
    {
        if (body.size() < sizeof(CPacketBody_S107F9))
        {
            ClearLatestData(VMF::VisionCommands::Measure); return;
        }

        CPacketBody_S107F9 pkt;
        std::memcpy(&pkt, body.data(), sizeof(pkt));

        if (pkt.nStatus != 1)
        {
            ClearLatestData(VMF::VisionCommands::Measure); return;
        }

        DataMap data;
        data[Z_FOCUS_VALUE] = pkt.cData[0];
        data[X_OFFSET] = pkt.cData[1];
        data[Y_OFFSET] = pkt.cData[2];
        data[ANGLE] = pkt.cData[3];
        data[AUTO_VISION_SETTING] = pkt.cData[4];
        SetLatestData(VMF::VisionCommands::Measure, data);
    }

    void VisionMemoryProcessor::HandleDeviceCheck(ByteArray body)
    {
        (void)body;
        ClearLatestData(VMF::VisionCommands::DeviceCheck);
    }

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
