#pragma once

#include "VMF_API.h"
#include "IVisionClient.h"
#include "Types.h"
#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace VMF
{
    class VMF_API CMockVisionClient : public IVisionClient
    {
    public:
        CMockVisionClient();
        virtual ~CMockVisionClient();

        VC::Status Initialize(const VisionConnectionConfig& config) override;

        VC::Status InitializeWithSharedController(
            std::shared_ptr<VC::Controller> sharedCtrl,
            const VisionConnectionConfig& config) override;

        void       Disconnect() override;
        bool       IsConnected() const override;

        // ── 3가지 핵심 Request 함수 (IVisionClient 인터페이스) ──
        bool SetInformationAsync(const StringMap& params) override;
        bool RequestResultAsync(const StringMap& params) override;
        bool MeasureAsync(const StringMap& params) override;

        DataMap    GetLatestData(VisionCommand type) const override;
        void       ClearLatestData(VisionCommand type) override;
        bool       IsValid(VisionCommand type) const override;
        bool       HasReceived(VisionCommand type) const override;

        void       InitializeRecvThread() override;

        // ── 3가지 핵심 On 콜백 ──
        void OnSetInformation(ByteArray body) override;
        void OnRequestResult(ByteArray body) override;
        void OnMeasure(ByteArray body) override;

        // 테스트 헬퍼
        void      SetRequestResult(bool ok);
        StringMap GetLastRequestParams()        const;

    private:
        mutable std::mutex       m_mutex;
        bool                     m_connected;
        bool                     m_requestResult;
        std::map<int, StringMap> m_latestData;
        std::map<int, bool>      m_receivedFlags;
        StringMap                m_lastRequestParams;

        std::shared_ptr<VC::Controller> m_sharedCtrl;

        static std::string BodyToString(const ByteArray& b);
    };
} // namespace VMF

