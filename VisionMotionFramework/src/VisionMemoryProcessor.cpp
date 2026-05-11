#include "stdafx.h"
#include "VisionMemoryProcessor.h"

#include "VisionMsgDispatcher.h"
#include "VisionLogger.h"
#include "SECSIPacket.h"
#include "VisionMemoryKeys.h"
#include "VisionPacketMemory.h"

#include <vector>
#include <cstring>

namespace VMF
{
    VisionMemoryProcessor::VisionMemoryProcessor()
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
                // ControlAck needs to call two handlers; make a copy for the second call
                auto copy = body; // copy before moving
                this->OnSetCok(std::move(body));
                this->OnInspReady(std::move(copy));
            });
    }

    // ?ㅻ뜑?먯꽌 ~VisionMemoryProcessor() override; 濡?蹂寃쏀븯?怨??ш린??紐낆떆??default ?ъ슜
    VisionMemoryProcessor::~VisionMemoryProcessor() = default;

    bool VisionMemoryProcessor::RequestSetCokAsync(const StringMap& params)
    {
        ClearLatestData(SetCok);

        CPacketBody_S2F41 body;
        body.nCmd =1000;
        body.nParamCount =7;

        // C++11: ???異붾줎 auto瑜??댁슜???ν솴??諛섎났??????좎뼵 異뺤냼
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

    bool VisionMemoryProcessor::RequestInspReadyAsync(const StringMap& params)
    {
        ClearLatestData(InspReady);

        CPacketBody_S2F41 body;
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

    bool VisionMemoryProcessor::RequestMeasureAsync(const StringMap& params)
    {
        ClearLatestData(Measure);

        CPacketBody_S107F9 body;

        auto it = params.find(CAMERA_ID);
        if (it != params.end()) body.nDataID = std::stoi(it->second); // C++11 std::stoi ?泥?

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

    bool VisionMemoryProcessor::RequestDeviceCheckAsync(const StringMap& params)
    {
        (void)params;
        return false;
    }

    bool VisionMemoryProcessor::RequestLightAsync(const StringMap& params)
    {
        (void)params;
        return false;
    }

    void VisionMemoryProcessor::OnSetCok(ByteArray body)
    {
        if (body.size() < sizeof(CPacketBody_S2F41))
        {
            ClearLatestData(SetCok);
            return;
        }

        CPacketBody_S2F41 pkt;
        std::memcpy(&pkt, body.data(), sizeof(pkt));

        DataMap data;
        data[RESULT]       = std::string(pkt.szParam[0]);
        data[SERVER_INDEX] = std::string(pkt.szParam[1]);
        data[CAM_STATUS]   = std::string(pkt.szParam[2]);
        data[CAM_TYPE]     = std::string(pkt.szParam[3]);

        SetLatestData(SetCok, data);
    }

    void VisionMemoryProcessor::OnInspReady(ByteArray body)
    {
        if (body.size() < sizeof(CPacketBody_S2F41))
        {
            ClearLatestData(InspReady);
            return;
        }

        CPacketBody_S2F41 pkt;
        std::memcpy(&pkt, body.data(), sizeof(pkt));

        DataMap data;
        // 諛곗뿴 ?ъ씤?곕줈遺??std::string ?앹꽦 媛뺤젣?섏뿬 ?명솚??媛뺥솕
        data[RESULT]       = std::string(pkt.szParam[0]);
        data[SERVER_INDEX] = std::string(pkt.szParam[1]);
        data[CAM_STATUS]   = std::string(pkt.szParam[2]);
        data[CAM_TYPE]     = std::string(pkt.szParam[3]);

        SetLatestData(InspReady, data);
    }

    std::vector<std::string> VisionMemoryProcessor::ParseMeasureBody(const ByteArray& body)
    {
        std::vector<std::string> results;
        if (body.size() < sizeof(CPacketBody_S107F9))
        {
            return results;
        }

        CPacketBody_S107F9 pkt;
        std::memcpy(&pkt, body.data(), sizeof(pkt));

        if (pkt.nStatus != 1)
        {
            return results;
        }

        results.push_back(pkt.cData[0]);
        results.push_back(pkt.cData[1]);
        results.push_back(pkt.cData[2]);
        results.push_back(pkt.cData[3]);
        results.push_back(pkt.cData[4]);

        return results;
    }

    void VisionMemoryProcessor::OnMeasure(ByteArray body)
    {
        if (body.size() < sizeof(CPacketBody_S107F9))
        {
            ClearLatestData(Measure);
            return;
        }

        CPacketBody_S107F9 pkt;
        std::memcpy(&pkt, body.data(), sizeof(pkt));

        if (pkt.nStatus != 1)
        {
            ClearLatestData(Measure);
            return;
        }

        DataMap data;
        data[Z_FOCUS_VALUE]       = pkt.cData[0];
        data[X_OFFSET]            = pkt.cData[1];
        data[Y_OFFSET]            = pkt.cData[2];
        data[ANGLE]               = pkt.cData[3];
        data[AUTO_VISION_SETTING] = pkt.cData[4];

        SetLatestData(Measure, data);
    }

    void VisionMemoryProcessor::Process()
    {
        VisionProcessorBase::Process();
    }

    void VisionMemoryProcessor::OnDeviceCheck(ByteArray body)
    {
        (void)body;
        ClearLatestData(DeviceCheck);
    }

    void VisionMemoryProcessor::OnLight(ByteArray body)
    {
        (void)body;
        ClearLatestData(Light);
    }
} 


