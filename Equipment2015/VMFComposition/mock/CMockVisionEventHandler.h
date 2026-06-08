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

        // IAsyncVisionProcessor
        virtual VC::Status Initialize(const VisionConnectionConfig& config);
        virtual void Disconnect();
        virtual bool IsConnected() const;

        virtual bool RequestSetCokAsync(const StringMap& params);
        virtual bool RequestInspReadyAsync(const StringMap& params);
        virtual bool RequestMeasureAsync(const StringMap& params);
        virtual bool RequestDeviceCheckAsync(const StringMap& params);
        virtual bool RequestLightAsync(const StringMap& params);

        virtual DataMap GetLatestData(VisionCommand type) const;
        virtual void ClearLatestData(VisionCommand type);
        virtual bool IsValid(VisionCommand type) const;
        virtual bool HasReceived(VisionCommand type) const;

        // IVisionProcessor (?섏떊 肄쒕갚)
        virtual void InitializeRecvThread();

        virtual void OnSetCok(ByteArray body);
        virtual void OnInspReady(ByteArray body);
        virtual void OnMeasure(ByteArray body);
        virtual void OnDeviceCheck(ByteArray body);
        virtual void OnLight(ByteArray body);

        // ?뚯뒪???ы띁
        void SetRequestResult(bool ok);
        StringMap GetLastRequestParams() const;

    private:
        mutable std::mutex m_mutex;
        bool m_connected;
        bool m_requestResult; // RequestXXXAsync 諛섑솚媛믪쓣 ?쒖뼱

        // 理쒖떊 ?곗씠??蹂닿? (VisionCommand -> StringMap)
        std::map<int, StringMap> m_latestData;

        // ?섏떊 ?뚮옒洹?(?몄뒪?댁뒪蹂?
        std::map<int, bool> m_receivedFlags;

        // 留덉?留됱쑝濡??꾨떖???붿껌 ?뚮씪誘명꽣 (?⑥닚 寃?ъ슜)
        StringMap m_lastRequestParams;

        // 諛붾뵒瑜?媛꾨떒??臾몄옄?대줈 蹂?섑븯???좏떥
        static std::string BodyToString(const ByteArray& b);
    };
} // namespace VMF


