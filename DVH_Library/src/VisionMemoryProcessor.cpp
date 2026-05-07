#include "stdafx.h"
#include "VisionMemoryProcessor.h"

#include "VisionMsgDispatcher.h"
#include "VisionLogger.h"
#include "SECSIPacket.h"
#include "VisionMemoryKeys.h"
#include "VisionPacketMemory.h"

#include <vector>
#include <cstring> // std::memcpy 사용 보장

namespace DVH_VAT
{
    VisionMemoryProcessor::VisionMemoryProcessor()
    {
        VisionCom::VisionMsgDispatcher& disp = m_ctrl.GetDispatcher();

        // 람다 함수 캡처 방식을 더욱 안정적/명확하게 처리할 수 있으나 현재 [this] 캡처는 유효함.
        disp.OnInspReady    = [this](const VisionCom::ByteArray& b) { this->OnInspReady(b);     };
        disp.OnSetCok       = [this](const VisionCom::ByteArray& b) { this->OnSetCok(b);        };
        disp.OnMeasure      = [this](const VisionCom::ByteArray& b) { this->OnMeasure(b);       };
        disp.OnLight        = [this](const VisionCom::ByteArray& b) { this->OnLight(b);         };
        disp.OnDeviceCheck  = [this](const VisionCom::ByteArray& b) { this->OnDeviceCheck(b);   };

        disp.RegisterHandler(107, 9,
            [this](int s, int f, const std::vector<uint8_t>& body, int serverIndex)
            {
                VisionCom::VisionMsgDispatcher& d = this->m_ctrl.GetDispatcher();

                if (d.OnMeasure) d.OnMeasure(body);
            });

        disp.RegisterHandler(2, 42,
            [this](int s, int f, const std::vector<uint8_t>& body, int serverIndex)
            {
                VisionCom::VisionMsgDispatcher& d = this->m_ctrl.GetDispatcher();
                if (d.OnSetCok)      d.OnSetCok(body);
                if (d.OnInspReady)   d.OnInspReady(body);
            });
    }

    // 헤더에서 ~VisionMemoryProcessor() override; 로 변경하였고 여기서 명시적 default 사용
    VisionMemoryProcessor::~VisionMemoryProcessor() = default;

    bool VisionMemoryProcessor::RequestSetCokAsync(const StringMap& params)
    {
        ClearLatestData(SetCok);

        CPacketBody_S2F41 body;
        body.nCmd = 1000;
        body.nParamCount = 7;

        // C++11: 타입 추론 auto를 이용해 장황한 반복자 타입 선언 축소
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

        auto it = params.find(CAMERA_ID);
        if (it != params.end()) body.nDataID = std::stoi(it->second); // C++11 std::stoi 대체

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
        data[RESULT]       = std::string(pkt.szParam[0]);
        data[SERVER_INDEX] = std::string(pkt.szParam[1]);
        data[CAM_STATUS]   = std::string(pkt.szParam[2]);
        data[CAM_TYPE]     = std::string(pkt.szParam[3]);

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
        // 배열 포인터로부터 std::string 생성 강제하여 호환성 강화
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

