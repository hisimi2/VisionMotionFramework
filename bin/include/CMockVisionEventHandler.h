#pragma once
#include "IVisionProcessor.h"
#include "Types.h"
#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace VMF
{
    class CMockVisionEventHandler : public IVisionProcessor
    {
    public:
        CMockVisionEventHandler();
        virtual ~CMockVisionEventHandler();

        // IVisionProcessor
        VC::Status Initialize(const VisionConnectionConfig& config) override;

        /// <summary>
        /// 외부에서 공유되는 Controller를 사용하여 초기화합니다.
        /// </summary>
        VC::Status InitializeWithSharedController(
            std::shared_ptr<VC::Controller> sharedCtrl,
            const VisionConnectionConfig& config) override;

        void       Disconnect() override;
        bool       IsConnected() const override;

        bool       RequestAsync(VisionCommand cmd,
        const StringMap& params) override;

        DataMap    GetLatestData(VisionCommand type) const override;
        void       ClearLatestData(VisionCommand type) override;
        bool       IsValid(VisionCommand type) const override;
        bool       HasReceived(VisionCommand type) const override;

        void       InitializeRecvThread() override;
        void       OnVisionResponse(VisionCommand cmd, ByteArray body) override;

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

        /// <summary>
        /// 공유 Controller (ConnectionManager로부터 획득)
        /// </summary>
        std::shared_ptr<VC::Controller> m_sharedCtrl;

        static std::string BodyToString(const ByteArray& b);
    };

} // namespace VMF
