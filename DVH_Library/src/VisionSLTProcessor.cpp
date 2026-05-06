#include "stdafx.h"
#include "VisionSLTProcessor.h"
#include "VisionMsgDispatcher.h"
#include "VisionPacketSLT.h"
#include "VisionMemoryKeys.h" // VisionKeys 사용을 위해 추가

#include <vector>
#include <string>
#include <cstring>

namespace DVH_VAT
{
    VisionSLTProcessor::VisionSLTProcessor()
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
                VisionCom::VisionMsgDispatcher& d = this->m_ctrl.GetDispatcher();
                if (d.OnMeasure) d.OnMeasure(body);
            });

        disp.RegisterHandler(2, 42,
            [this](int s, int f, const std::vector<uint8_t>& body, int serverIndex)
            {
                VisionCom::VisionMsgDispatcher& d = this->m_ctrl.GetDispatcher();
                if (d.OnSetCok) d.OnSetCok(body);
            });

        disp.RegisterHandler(2, 4,
            [this](int s, int f, const std::vector<uint8_t>& body, int serverIndex)
            {
                VisionCom::VisionMsgDispatcher& d = this->m_ctrl.GetDispatcher();
                if (d.OnDeviceCheck) d.OnDeviceCheck(body);
            });
    }

    // 헤더에서 명시적 override 및 default 처리 연계
    VisionSLTProcessor::~VisionSLTProcessor() = default;

    bool VisionSLTProcessor::RequestSetCokAsync(const StringMap& params)
    {
        ClearLatestData(SetCok);

        CPacketBody_S2F41 body;
        // memset 대신 Clear() 유틸 사용 권장
        body.Clear();
        body.nCmd = 1000;
        body.nParamCount = 7;

        // C++11: auto를 이용한 타입 추론
        auto it = params.find(VisionKeys::RECIPE_NAME);
        if (it != params.end()) strncpy_s(body.szParam[1], STR_LEN, it->second.c_str(), _TRUNCATE);
        
        it = params.find(VisionKeys::PCD_MODE);
        if (it != params.end()) strncpy_s(body.szParam[2], STR_LEN, it->second.c_str(), _TRUNCATE);
        
        it = params.find(VisionKeys::DEVICE_SIZE_X);
        if (it != params.end()) strncpy_s(body.szParam[3], STR_LEN, it->second.c_str(), _TRUNCATE);
        
        it = params.find(VisionKeys::DEVICE_SIZE_Y);
        if (it != params.end()) strncpy_s(body.szParam[4], STR_LEN, it->second.c_str(), _TRUNCATE);
        
        it = params.find(VisionKeys::COK_TYPE);
        if (it != params.end()) strncpy_s(body.szParam[5], STR_LEN, it->second.c_str(), _TRUNCATE);
        
        it = params.find(VisionKeys::PICKER_PITCH_X);
        if (it != params.end()) strncpy_s(body.szParam[6], STR_LEN, it->second.c_str(), _TRUNCATE);
        
        it = params.find(VisionKeys::PICKER_PITCH_Y);
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

    bool VisionSLTProcessor::RequestInspReadyAsync(const StringMap& params)
    {
        ClearLatestData(InspReady);

        CPacketBody_S2F41 body;
        body.Clear();
        body.nCmd = 1000;
        body.nParamCount = 7;

        auto it = params.find(VisionKeys::RECIPE_NAME);
        if (it != params.end()) strncpy_s(body.szParam[1], STR_LEN, it->second.c_str(), _TRUNCATE);
        
        it = params.find(VisionKeys::PCD_MODE);
        if (it != params.end()) strncpy_s(body.szParam[2], STR_LEN, it->second.c_str(), _TRUNCATE);
        
        it = params.find(VisionKeys::DEVICE_SIZE_X);
        if (it != params.end()) strncpy_s(body.szParam[3], STR_LEN, it->second.c_str(), _TRUNCATE);
        
        it = params.find(VisionKeys::DEVICE_SIZE_Y);
        if (it != params.end()) strncpy_s(body.szParam[4], STR_LEN, it->second.c_str(), _TRUNCATE);
        
        it = params.find(VisionKeys::COK_TYPE);
        if (it != params.end()) strncpy_s(body.szParam[5], STR_LEN, it->second.c_str(), _TRUNCATE);
        
        it = params.find(VisionKeys::PICKER_PITCH_X);
        if (it != params.end()) strncpy_s(body.szParam[6], STR_LEN, it->second.c_str(), _TRUNCATE);
        
        it = params.find(VisionKeys::PICKER_PITCH_Y);
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

    bool VisionSLTProcessor::RequestMeasureAsync(const StringMap& params)
    {
        ClearLatestData(Measure);

        CPacketBody_S107F9 body;
        body.Clear();

        // C++11: atoi 대신 std::stoi
        auto it = params.find(VisionKeys::CAMERA_ID);
        if (it != params.end()) body.nDataID = std::stoi(it->second);

        it = params.find(VisionKeys::INSPECTION_TYPE);
        if (it != params.end()) body.nStatus = std::stoi(it->second);

        it = params.find(VisionKeys::MOVE_PART);
        if (it != params.end()) body.SetData(0, it->second.c_str());

        it = params.find(VisionKeys::SAVE_IMAGE);
        if (it != params.end()) body.SetData(1, it->second.c_str());

        it = params.find(VisionKeys::FOV_DIRECTION);
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

    void VisionSLTProcessor::OnSetCok(const ByteArray& body)
    {
        if (body.size() < sizeof(CPacketBody_S2F42))
        {
            ClearLatestData(SetCok);
            return;
        }

        CPacketBody_S2F42 pkt;
        std::memcpy(&pkt, body.data(), sizeof(pkt));

        DataMap data;
        // C++11: std::stringstream 대신 std::to_string 사용 (성능/가독성 향상)
        data["MsgID"]   = std::to_string(pkt.nDataID);
        data["PCIndex"] = std::to_string(pkt.nIndex);
        data["Status"]  = std::to_string(pkt.nRCMDACK);

        SetLatestData(SetCok, data);
    }

    void VisionSLTProcessor::OnInspReady(const ByteArray& body)
    {
        if (body.size() < sizeof(CPacketBody_S2F42))
        {
            ClearLatestData(InspReady);
            return;
        }

        CPacketBody_S2F42 pkt;
        std::memcpy(&pkt, body.data(), sizeof(pkt));

        DataMap data;
        // C++11: std::stringstream 대체
        data["MsgID"]   = std::to_string(pkt.nDataID);
        data["PCIndex"] = std::to_string(pkt.nIndex);
        data["Result"]  = std::to_string(pkt.nRCMDACK);

        SetLatestData(InspReady, data);
    }

    void VisionSLTProcessor::OnMeasure(const ByteArray& body)
    {
        if (body.size() < sizeof(CPacketBody_S107F9))
        {
            ClearLatestData(Measure);
            return;
        }

        CPacketBody_S107F9 packet;
        std::memcpy(&packet, body.data(), sizeof(packet));

        if (packet.nStatus != VisionCom::VisionOK)
        {
            ClearLatestData(Measure);
            return;
        }

        DataMap data;
        data["Error Description"] = std::string(packet.cData[0]);
        data[VisionKeys::X_OFFSET] = std::string(packet.cData[1]);
        data[VisionKeys::Y_OFFSET] = std::string(packet.cData[2]);
        data["ZFocus"]            = std::string(packet.cData[3]);

        SetLatestData(Measure, data);
    }

    void VisionSLTProcessor::OnDeviceCheck(const ByteArray& body)
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

    void VisionSLTProcessor::OnLight(const ByteArray& body)
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
} // namespace DVH_VAT

