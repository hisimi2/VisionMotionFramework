#include "stdafx.h"
#include "CMockVisionEventHandler.h"

namespace VMF
{
    CMockVisionEventHandler::CMockVisionEventHandler()
        : m_connected(false)
        , m_requestResult(true)
    {
        // m_latestData, m_receivedFlags 湲곕낯 ?앹꽦???ъ슜
    }

    CMockVisionEventHandler::~CMockVisionEventHandler()
    {
    }

    VisionComm::Status CMockVisionEventHandler::Initialize(const VisionConnectionConfig& /*config*/)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_connected = true;
        return VisionComm::VisionOK;
    }

    void CMockVisionEventHandler::Disconnect()
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_connected = false;
    }

    bool CMockVisionEventHandler::IsConnected() const
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        return m_connected;
    }

    bool CMockVisionEventHandler::RequestSetCokAsync(const StringMap& params)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_lastRequestParams = params;
        return m_requestResult;
    }

    bool CMockVisionEventHandler::RequestInspReadyAsync(const StringMap& params)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_lastRequestParams = params;
        return m_requestResult;
    }

    bool CMockVisionEventHandler::RequestMeasureAsync(const StringMap& params)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_lastRequestParams = params;
        return m_requestResult;
    }

    bool CMockVisionEventHandler::RequestDeviceCheckAsync(const StringMap& params)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_lastRequestParams = params;
        return m_requestResult;
    }

    bool CMockVisionEventHandler::RequestLightAsync(const StringMap& params)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_lastRequestParams = params;
        return m_requestResult;
    }

    CMockVisionEventHandler::DataMap CMockVisionEventHandler::GetLatestData(VatCommand type) const
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        int key = type;
        std::map<int, StringMap>::const_iterator it = m_latestData.find(key);
        if (it != m_latestData.end()) return it->second;
        return DataMap();
    }

    void CMockVisionEventHandler::ClearLatestData(VatCommand type)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_latestData.erase(type);
        m_receivedFlags[type] = false;
    }

    bool CMockVisionEventHandler::IsValid(VatCommand type) const
    {
		return true;
    }

    bool CMockVisionEventHandler::HasReceived(VatCommand type) const
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        int key = type;
        std::map<int, bool>::const_iterator it = m_receivedFlags.find(key);
        return (it != m_receivedFlags.end()) && it->second;
    }

    void CMockVisionEventHandler::InitializeRecvThread()
    {
        // ?뚯뒪?몄슜 紐⑥쓽 援ы쁽: ?섏떊 ?ㅻ젅???놁쓬
    }

    void CMockVisionEventHandler::OnSetCok(ByteArray body)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        StringMap m;
        m["body"] = BodyToString(body);
        m_latestData[SetCok] = m;
        m_receivedFlags[SetCok] = true;
    }

    void CMockVisionEventHandler::OnInspReady(ByteArray body)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        StringMap m;
        m["body"] = BodyToString(body);
        m_latestData[InspReady] = m;
        m_receivedFlags[InspReady] = true;
    }

    void CMockVisionEventHandler::OnMeasure(ByteArray body)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        StringMap m;
        m["body"] = BodyToString(body);
        m_latestData[Measure] = m;
        m_receivedFlags[Measure] = true;
    }

    void CMockVisionEventHandler::OnDeviceCheck(ByteArray body)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        StringMap m;
        m["body"] = BodyToString(body);
        m_latestData[DeviceCheck] = m;
        m_receivedFlags[DeviceCheck] = true;
    }

    void CMockVisionEventHandler::OnLight(ByteArray body)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        StringMap m;
        m["body"] = BodyToString(body);
        m_latestData[Light] = m;
        m_receivedFlags[Light] = true;
    }

    void CMockVisionEventHandler::SetRequestResult(bool ok)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_requestResult = ok;
    }

    StringMap CMockVisionEventHandler::GetLastRequestParams() const
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        return m_lastRequestParams;
    }

    std::string CMockVisionEventHandler::BodyToString(const ByteArray& b)
    {
        if (b.empty()) return std::string();
        return std::string(reinterpret_cast<const char*>(b.data()), b.size());
    }

} // namespace VMF

