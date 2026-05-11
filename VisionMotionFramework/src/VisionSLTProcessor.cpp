#include "stdafx.h"
#include "VisionSLTProcessor.h"
#include "VisionMsgDispatcher.h"
#include "VisionPacketSLT.h"
#include "VisionMemoryKeys.h"

#include <vector>
#include <string>
#include <cstring>

namespace VMF
{
    VisionSLTProcessor::VisionSLTProcessor()
    {
        VisionComm::VisionMsgDispatcher& disp = m_ctrl.GetDispatcher();

        disp.RegisterHandler(VisionComm::VisionProtocol::Measure,
            [this](int s, int f, std::vector<uint8_t>&& body, int serverIndex)
            {
                (void)s;
                (void)f;
                (void)serverIndex;
                this->OnMeasure(std::move(body));
            });

        disp.RegisterHandler(VisionComm::VisionProtocol::ControlAck,
            [this](int s, int f, std::vector<uint8_t>&& body, int serverIndex)
            {
                (void)s;
                (void)f;
                (void)serverIndex;
                this->OnSetCok(std::move(body));
            });

        disp.RegisterHandler(VisionComm::VisionProtocol::DeviceCheckAck,
            [this](int s, int f, std::vector<uint8_t>&& body, int serverIndex)
            {
                (void)s;
                (void)f;
                (void)serverIndex;
                this->OnDeviceCheck(std::move(body));
            });
    }

    // ?ㅻ뜑?먯꽌 紐낆떆??override 諛?default 泥섎━ ?곌퀎
    VisionSLTProcessor::~VisionSLTProcessor() = default;

    bool VisionSLTProcessor::RequestSetCokAsync(const StringMap& params)
    {
        ClearLatestData(SetCok);

        CPacketBody_S2F41 body;
        // memset ???Clear() ?좏떥 ?ъ슜 沅뚯옣
        body.Clear();
        body.nCmd =1000;
        body.nParamCount =7;

        // C++11: auto瑜??댁슜?????異붾줎
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

        VisionComm::SECSPacket secsPkt;
        secsPkt.SetCorrelationId(body.nCmd);
        secsPkt.SetProtocol(VisionComm::VisionProtocol::ControlRequest);
        secsPkt.SetBody(bodyBytes);

        return (m_ctrl.SendPacketAsync(secsPkt) == VisionComm::VisionOK);
    }

    bool VisionSLTProcessor::RequestInspReadyAsync(const StringMap& params)
    {
        ClearLatestData(InspReady);

        CPacketBody_S2F41 body;
        body.Clear();
        body.nCmd =1000;
        body.nParamCount =7;

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

        VisionComm::SECSPacket secsPkt;
        secsPkt.SetCorrelationId(body.nCmd);
        secsPkt.SetProtocol(VisionComm::VisionProtocol::ControlRequest);
        secsPkt.SetBody(bodyBytes);

        return (m_ctrl.SendPacketAsync(secsPkt) == VisionComm::VisionOK);
    }

    bool VisionSLTProcessor::RequestMeasureAsync(const StringMap& params)
    {
        ClearLatestData(Measure);

        CPacketBody_S107F9 body;
        body.Clear();

        // C++11: atoi ???std::stoi
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

        VisionComm::SECSPacket secsPkt;
        secsPkt.SetCorrelationId(body.nDataID);
        secsPkt.SetProtocol(VisionComm::VisionProtocol::Measure);
        secsPkt.SetBody(bodyBytes);

        return (m_ctrl.SendPacketAsync(secsPkt) == VisionComm::VisionOK);
    }

    bool VisionSLTProcessor::RequestDeviceCheckAsync(const StringMap& params)
    {
        (void)params;
        return false;
    }

    bool VisionSLTProcessor::RequestLightAsync(const StringMap& params)
    {
        (void)params;
        return false;
    }

    void VisionSLTProcessor::OnSetCok(ByteArray body)
    {
        if (body.size() < sizeof(CPacketBody_S2F42))
        {
            ClearLatestData(SetCok);
            return;
        }

        CPacketBody_S2F42 pkt;
        std::memcpy(&pkt, body.data(), sizeof(pkt));

        DataMap data;
        data["MsgID"]   = std::to_string(pkt.nDataID);
        data["PCIndex"] = std::to_string(pkt.nIndex);
        data["Status"]  = std::to_string(pkt.nRCMDACK);

        SetLatestData(SetCok, data);
    }

    void VisionSLTProcessor::OnInspReady(ByteArray body)
    {
        if (body.size() < sizeof(CPacketBody_S2F42))
        {
            ClearLatestData(InspReady);
            return;
        }

        CPacketBody_S2F42 pkt;
        std::memcpy(&pkt, body.data(), sizeof(pkt));

        DataMap data;
        data["MsgID"]   = std::to_string(pkt.nDataID);
        data["PCIndex"] = std::to_string(pkt.nIndex);
        data["Result"]  = std::to_string(pkt.nRCMDACK);

        SetLatestData(InspReady, data);
    }

    void VisionSLTProcessor::OnMeasure(ByteArray body)
    {
        if (body.size() < sizeof(CPacketBody_S107F9))
        {
            ClearLatestData(Measure);
            return;
        }

        CPacketBody_S107F9 packet;
        std::memcpy(&packet, body.data(), sizeof(packet));

        if (packet.nStatus != VisionComm::VisionOK)
        {
            ClearLatestData(Measure);
            return;
        }

        DataMap data;
        data["Error Description"] = std::string(packet.cData[0]);
        data[X_OFFSET] = std::string(packet.cData[1]);
        data[Y_OFFSET] = std::string(packet.cData[2]);
        data["ZFocus"]            = std::string(packet.cData[3]);

        SetLatestData(Measure, data);
    }

    void VisionSLTProcessor::OnDeviceCheck(ByteArray body)
    {
        if (body.size() < sizeof(CPacketBody_S2F4))
        {
            ClearLatestData(DeviceCheck);
            return;
        }

        CPacketBody_S2F4 packet;
        std::memcpy(&packet, body.data(), sizeof(packet));

        DataMap data;
        data["MsgID"]      = std::to_string(packet.nDataID);
        data["Status"]     = std::to_string(packet.nStatus);
        data["Data Count"] = std::to_string(packet.nDataCount);

        data["Error Description"]        = std::string(packet.cData[0]);
        data["Device Check Result Data"] = std::string(packet.cData[1]);

        SetLatestData(DeviceCheck, data);
    }

    void VisionSLTProcessor::OnLight(ByteArray body)
    {
        if (body.size() < sizeof(CPacketBody_S2F42))
        {
            ClearLatestData(Light);
            return;
        }

        CPacketBody_S2F42 pkt;
        std::memcpy(&pkt, body.data(), sizeof(pkt));

        DataMap data;
        data["MsgID"]   = std::to_string(pkt.nDataID);
        data["PCIndex"] = std::to_string(pkt.nIndex);
        data["Status"]  = std::to_string(pkt.nRCMDACK);

        SetLatestData(Light, data);
    }

    void VisionSLTProcessor::Process()
    {
        VisionProcessorBase::Process();
    }
} // namespace VMF


