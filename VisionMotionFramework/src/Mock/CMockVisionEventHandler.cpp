#include "stdafx.h"
#include "Mock/CMockVisionEventHandler.h"

using namespace VMF::VisionCommands;

namespace VMF
{
    CMockVisionEventHandler::CMockVisionEventHandler()
        : m_connected(false)
        , m_requestResult(true)
    {
    }

    CMockVisionEventHandler::~CMockVisionEventHandler()
    {
    }

    VC::Status CMockVisionEventHandler::Initialize(
        const VisionConnectionConfig& /*config*/)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_connected = true;
        return VC::VisionOK;
    }

    VC::Status CMockVisionEventHandler::InitializeWithSharedController(
        std::shared_ptr<VC::Controller> sharedCtrl,
        const VisionConnectionConfig& /*config*/)
    {
        std::lock_guard<std::mutex> lg(m_mutex);

        if (!sharedCtrl)
        {
            return VC::VisionConnectionFailed;
        }

        m_sharedCtrl = sharedCtrl;
        m_connected = true;

        return VC::VisionOK;
    }

    void CMockVisionEventHandler::Disconnect()
    {
        std::lock_guard<std::mutex> lg(m_mutex);

        if (m_sharedCtrl)
        {
            // 공유 Controller는 ConnectionManager가 관리하므로 직접 disconnect하지 않음
            m_sharedCtrl.reset();
        }

        m_connected = false;
    }

    bool CMockVisionEventHandler::IsConnected() const
    {
        std::lock_guard<std::mutex> lg(m_mutex);

        // Mock: 공유 Controller 여부와 관계없이 자체 연결 상태 반환
        // (실제 Controller 메서드에 의존하지 않음)
        return m_connected;
    }

    // -----------------------------------------------------------------------
    // [RequestAsync] — cmd로 분기
    // -----------------------------------------------------------------------
    bool CMockVisionEventHandler::RequestAsync(
        VisionCommand cmd, const StringMap& params)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_lastRequestParams = params;
        return m_requestResult;
    }

    // -----------------------------------------------------------------------
    // [OnVisionResponse] — cmd로 분기
    // -----------------------------------------------------------------------
    void CMockVisionEventHandler::OnVisionResponse(
        VisionCommand cmd, ByteArray body)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        StringMap m;
        m["body"] = BodyToString(body);
        m_latestData[cmd] = m;
        m_receivedFlags[cmd] = true;
    }

    // -----------------------------------------------------------------------
    // [데이터 조회]
    // -----------------------------------------------------------------------
    CMockVisionEventHandler::DataMap CMockVisionEventHandler::GetLatestData(
        VisionCommand type) const
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        std::map<int, StringMap>::const_iterator it =
            m_latestData.find(type);
        return (it != m_latestData.end()) ? it->second : DataMap();
    }

    void CMockVisionEventHandler::ClearLatestData(VisionCommand type)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_latestData.erase(type);
        m_receivedFlags[type] = false;
    }

    bool CMockVisionEventHandler::IsValid(VisionCommand type) const
    {
        return true;
    }

    bool CMockVisionEventHandler::HasReceived(VisionCommand type) const
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        std::map<int, bool>::const_iterator it =
            m_receivedFlags.find(type);
        return (it != m_receivedFlags.end()) && it->second;
    }

    void CMockVisionEventHandler::InitializeRecvThread()
    {
        // Mock — 실제 수신 스레드 없음
    }

    // -----------------------------------------------------------------------
    // [테스트 헬퍼]
    // -----------------------------------------------------------------------
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
