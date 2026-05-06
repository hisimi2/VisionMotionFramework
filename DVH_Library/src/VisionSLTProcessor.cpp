#include "stdafx.h"
#include "VisionSLTProcessor.h"
#include "VisionMsgDispatcher.h"
#include "VisionPacketSLT.h"

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
                VisionCom::VisionMsgDispatcher& disp = this->m_ctrl.GetDispatcher();
                if (disp.OnMeasure) disp.OnMeasure(body);
            });

        disp.RegisterHandler(2, 42,
            [this](int s, int f, const std::vector<uint8_t>& body, int serverIndex)
            {
                VisionCom::VisionMsgDispatcher& disp = this->m_ctrl.GetDispatcher();
                if (disp.OnSetCok) disp.OnSetCok(body);
            });

        disp.RegisterHandler(2, 4,
            [this](int s, int f, const std::vector<uint8_t>& body, int serverIndex)
            {
                VisionCom::VisionMsgDispatcher& disp = this->m_ctrl.GetDispatcher();
                if (disp.OnDeviceCheck) disp.OnDeviceCheck(body);
            });
    }

    VisionSLTProcessor::~VisionSLTProcessor()
    {
    }

    bool VisionSLTProcessor::RequestSetCokAsync(const StringMap& params)
    {
        ClearLatestData(SetCok);

        CPacketBody_S2F41 body;
        std::memset(&body, 0, sizeof(body));
        body.nCmd = 1000;
        body.nParamCount = 7;

        StringMap::const_iterator it = params.find("recipe_name");
        if (it != params.end()) strncpy_s(body.szParam[1], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find("pcd_mode");
        if (it != params.end()) strncpy_s(body.szParam[2], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find("device_size_x");
        if (it != params.end()) strncpy_s(body.szParam[3], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find("device_size_y");
        if (it != params.end()) strncpy_s(body.szParam[4], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find("cok_type");
        if (it != params.end()) strncpy_s(body.szParam[5], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find("picker_pitch_x");
        if (it != params.end()) strncpy_s(body.szParam[6], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find("picker_pitch_y");
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
        std::memset(&body, 0, sizeof(body));
        body.nCmd = 1000;
        body.nParamCount = 7;

        StringMap::const_iterator it = params.find("recipe_name");
        if (it != params.end()) strncpy_s(body.szParam[1], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find("pcd_mode");
        if (it != params.end()) strncpy_s(body.szParam[2], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find("device_size_x");
        if (it != params.end()) strncpy_s(body.szParam[3], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find("device_size_y");
        if (it != params.end()) strncpy_s(body.szParam[4], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find("cok_type");
        if (it != params.end()) strncpy_s(body.szParam[5], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find("picker_pitch_x");
        if (it != params.end()) strncpy_s(body.szParam[6], STR_LEN, it->second.c_str(), _TRUNCATE);
        it = params.find("picker_pitch_y");
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

        StringMap::const_iterator it = params.find("CameraID");
        if (it != params.end()) body.nDataID = atoi(it->second.c_str());

        it = params.find("InspectionType");
        if (it != params.end()) body.nStatus = atoi(it->second.c_str());

        it = params.find("nMovePart");
        if (it != params.end()) body.SetData(0, it->second.c_str());

        it = params.find("bSaveImage");
        if (it != params.end()) body.SetData(1, it->second.c_str());

        it = params.find("nFovDirection");
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
        std::stringstream ss;
        ss << pkt.nDataID; data["MsgID"] = ss.str(); ss.str(""); ss.clear();
        ss << pkt.nIndex; data["PCIndex"] = ss.str(); ss.str(""); ss.clear();
        ss << pkt.nRCMDACK; data["Status"] = ss.str();

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
        std::stringstream ss;
        ss << pkt.nDataID; data["MsgID"] = ss.str(); ss.str(""); ss.clear();
        ss << pkt.nIndex; data["PCIndex"] = ss.str(); ss.str(""); ss.clear();
        ss << pkt.nRCMDACK; data["Result"] = ss.str();

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
        data["XOffset"] = std::string(packet.cData[1]);
        data["YOffset"] = std::string(packet.cData[2]);
        data["ZFocus"] = std::string(packet.cData[3]);

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
        std::stringstream ss;
        ss << packet.nDataID; data["MsgID"] = ss.str(); ss.str(""); ss.clear();
        ss << packet.nStatus; data["Status"] = ss.str(); ss.str(""); ss.clear();
        ss << packet.nDataCount; data["Data Count"] = ss.str();

        data["Error Description"] = std::string(packet.cData[0]);
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
        std::stringstream ss;
        ss << pkt.nDataID; data["MsgID"] = ss.str(); ss.str(""); ss.clear();
        ss << pkt.nIndex; data["PCIndex"] = ss.str(); ss.str(""); ss.clear();
        ss << pkt.nRCMDACK; data["Status"] = ss.str();

        SetLatestData(Light, data);
    }

    void VisionSLTProcessor::Process()
    {
        VisionProcessorBase::Process();
    }
} // namespace DVH_VAT

