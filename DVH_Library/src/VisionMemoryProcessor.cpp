#include "stdafx.h"
#include "VisionMemoryProcessor.h"

#include "VisionMsgDispatcher.h"
#include "VisionLogger.h"
#include "SECSIPacket.h"
#include "VisionMemoryKeys.h"
#include "VisionPacketMemory.h"

#include <vector>

namespace DVH_VAT
{
    VisionMemoryProcessor::VisionMemoryProcessor()
    {
        VisionCom::VisionMsgDispatcher& disp = m_ctrl.GetDispatcher();

        disp.OnInspReady    = [this](const VisionCom::ByteArray& b) { this->OnInspReady(b);     };
        disp.OnSetCok       = [this](const VisionCom::ByteArray& b) { this->OnSetCok(b);        };
        disp.OnMeasure      = [this](const VisionCom::ByteArray& b) { this->OnMeasure(b);       };
        disp.OnLight        = [this](const VisionCom::ByteArray& b) { this->OnLight(b);         };
        disp.OnDeviceCheck  = [this](const VisionCom::ByteArray& b) { this->OnDeviceCheck(b);   };

        disp.RegisterHandler(107, 9,
            [this](int s, int f, const std::vector<uint8_t>& body, int serverIndex)
            {
                VisionCom::VisionMsgDispatcher& disp = this->m_ctrl.GetDispatcher();

                if (disp.OnMeasure) disp.OnMeasure(body);
            });

        disp.RegisterHandler(2, 42,
            [this](int s, int f, const std::vector<uint8_t>& body, int serverIndex)
            {
                VisionCom::VisionMsgDispatcher& disp = this->m_ctrl.GetDispatcher();
                if (disp.OnSetCok)      disp.OnSetCok(body);
                if (disp.OnInspReady)   disp.OnInspReady(body);
            });
    }

    VisionMemoryProcessor::~VisionMemoryProcessor()
    {
    }

    bool VisionMemoryProcessor::RequestSetCokAsync(const StringMap& params)
    {
        ClearLatestData(SetCok);

        CPacketBody_S2F41 body;
        body.nCmd = 1000;
        body.nParamCount = 7;

        StringMap::const_iterator it = params.find(VAT_VISION_KEY_RECIPE_NAME);
        if (it != params.end()) strncpy_s(body.szParam[1], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(VAT_VISION_KEY_PCD_MODE);
        if (it != params.end()) strncpy_s(body.szParam[2], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(VAT_VISION_KEY_DEVICE_SIZE_X);
        if (it != params.end()) strncpy_s(body.szParam[3], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(VAT_VISION_KEY_DEVICE_SIZE_Y);
        if (it != params.end()) strncpy_s(body.szParam[4], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(VAT_VISION_KEY_COK_TYPE);
        if (it != params.end()) strncpy_s(body.szParam[5], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(VAT_VISION_KEY_PICKER_PITCH_X);
        if (it != params.end()) strncpy_s(body.szParam[6], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(VAT_VISION_KEY_PICKER_PITCH_Y);
        if (it != params.end()) strncpy_s(body.szParam[7], STR_LEN, it->second.c_str(), _TRUNCATE);

        std::vector<uint8_t> bodyBytes;
        const uint8_t* p = reinterpret_cast<const uint8_t*>(&body);
        bodyBytes.assign(p, p + sizeof(body));

        VisionCom::SECSPacket secsPkt;
        secsPkt.SetCorrelationId(body.nCmd);
        secsPkt.SetOpCode(2);
        secsPkt.SetSubCode(41);
        secsPkt.SetBody(bodyBytes);

        return (m_ctrl.SendPacketAsync(secsPkt) == VisionCom::VisionOK);
    }

    bool VisionMemoryProcessor::RequestInspReadyAsync(const StringMap& params)
    {
        ClearLatestData(InspReady);

        CPacketBody_S2F41 body;
        body.nCmd = 1000;
        body.nParamCount = 7;

        StringMap::const_iterator it = params.find(VAT_VISION_KEY_RECIPE_NAME);
        if (it != params.end()) strncpy_s(body.szParam[1], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(VAT_VISION_KEY_PCD_MODE);
        if (it != params.end()) strncpy_s(body.szParam[2], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(VAT_VISION_KEY_DEVICE_SIZE_X);
        if (it != params.end()) strncpy_s(body.szParam[3], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(VAT_VISION_KEY_DEVICE_SIZE_Y);
        if (it != params.end()) strncpy_s(body.szParam[4], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(VAT_VISION_KEY_COK_TYPE);
        if (it != params.end()) strncpy_s(body.szParam[5], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(VAT_VISION_KEY_PICKER_PITCH_X);
        if (it != params.end()) strncpy_s(body.szParam[6], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find(VAT_VISION_KEY_PICKER_PITCH_Y);
        if (it != params.end()) strncpy_s(body.szParam[7], STR_LEN, it->second.c_str(), _TRUNCATE);

        std::vector<uint8_t> bodyBytes;
        const uint8_t* p = reinterpret_cast<const uint8_t*>(&body);
        bodyBytes.assign(p, p + sizeof(body));

        VisionCom::SECSPacket secsPkt;
        secsPkt.SetCorrelationId(body.nCmd);
        secsPkt.SetOpCode(2);
        secsPkt.SetSubCode(41);
        secsPkt.SetBody(bodyBytes);

        return (m_ctrl.SendPacketAsync(secsPkt) == VisionCom::VisionOK);
    }

    bool VisionMemoryProcessor::RequestMeasureAsync(const StringMap& params)
    {
        ClearLatestData(Measure);

        CPacketBody_S107F9 body;

        StringMap::const_iterator it = params.find(VAT_VISION_KEY_CAMERA_ID);
        if (it != params.end()) body.nDataID = atoi(it->second.c_str());

        it = params.find(VAT_VISION_KEY_INSPECTION_TYPE);
        if (it != params.end()) body.nStatus = atoi(it->second.c_str());

        it = params.find(VAT_VISION_KEY_MOVE_PART);
        if (it != params.end()) body.SetData(0, it->second.c_str());

        it = params.find(VAT_VISION_KEY_SAVE_IMAGE);
        if (it != params.end()) body.SetData(1, it->second.c_str());

        it = params.find(VAT_VISION_KEY_FOV_DIRECTION);
        if (it != params.end()) body.SetData(3, it->second.c_str());

        std::vector<uint8_t> bodyBytes;
        const uint8_t* p = reinterpret_cast<const uint8_t*>(&body);
        bodyBytes.assign(p, p + sizeof(body));

        VisionCom::SECSPacket secsPkt;
        secsPkt.SetCorrelationId(body.nDataID);
        secsPkt.SetOpCode(107);
        secsPkt.SetSubCode(9);
        secsPkt.SetBody(bodyBytes);

        return (m_ctrl.SendPacketAsync(secsPkt) == VisionCom::VisionOK);
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

    void VisionMemoryProcessor::OnSetCok(const ByteArray& body)
    {
        if (body.size() < sizeof(CPacketBody_S2F41))
        {
            ClearLatestData(SetCok);
            return;
        }

        CPacketBody_S2F41 pkt;
        std::memcpy(&pkt, body.data(), sizeof(pkt));

        DataMap data;
        data[VAT_VISION_KEY_RESULT] = std::string(pkt.szParam[0]);
        data[VAT_VISION_KEY_SERVER_INDEX] = std::string(pkt.szParam[1]);
        data[VAT_VISION_KEY_CAM_STATUS] = std::string(pkt.szParam[2]);
        data[VAT_VISION_KEY_CAM_TYPE] = std::string(pkt.szParam[3]);

        SetLatestData(SetCok, data);
    }

    void VisionMemoryProcessor::OnInspReady(const ByteArray& body)
    {
        if (body.size() < sizeof(CPacketBody_S2F41))
        {
            ClearLatestData(InspReady);
            return;
        }

        CPacketBody_S2F41 pkt;
        std::memcpy(&pkt, body.data(), sizeof(pkt));

        DataMap data;
        data[VAT_VISION_KEY_RESULT] = pkt.szParam[0];
        data[VAT_VISION_KEY_SERVER_INDEX] = pkt.szParam[1];
        data[VAT_VISION_KEY_CAM_STATUS] = pkt.szParam[2];
        data[VAT_VISION_KEY_CAM_TYPE] = pkt.szParam[3];

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

    void VisionMemoryProcessor::OnMeasure(const ByteArray& body)
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
        data[VAT_VISION_KEY_Z_FOCUS_VALUE]          = pkt.cData[0];
        data[VAT_VISION_KEY_X_OFFSET]               = pkt.cData[1];
        data[VAT_VISION_KEY_Y_OFFSET]               = pkt.cData[2];
        data[VAT_VISION_KEY_ANGLE]                  = pkt.cData[3];
        data[VAT_VISION_KEY_AUTO_VISION_SETTING]    = pkt.cData[4];

        SetLatestData(Measure, data);
    }

    void VisionMemoryProcessor::Process()
    {
        VisionProcessorBase::Process();
    }

    void VisionMemoryProcessor::OnDeviceCheck(const ByteArray& body)
    {
        (void)body;
        ClearLatestData(DeviceCheck);
    }

    void VisionMemoryProcessor::OnLight(const ByteArray& body)
    {
        (void)body;
        ClearLatestData(Light);
    }
} 

