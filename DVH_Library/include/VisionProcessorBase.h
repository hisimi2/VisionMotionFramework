#pragma once

#include <memory>
#include <vector>

#include "DVH_VAT_API.h"
#include "IVisionProcessor.h"
#include "IResultSink.h"
#include "VisionController.h"

namespace DVH_VAT
{
    class DVH_VAT_API VisionProcessorBase : public IVisionEventHandler
    {
    public:
        typedef StringMap DataMap;

        VisionProcessorBase();
        virtual ~VisionProcessorBase();

        void Start();
        void Stop();
        void InitializeRecvThread();

        void RunLoop();
        virtual void Process();

        virtual VisionCom::VisionStatus Initialize(const VisionConnectionConfig& config);
        virtual void Disconnect();
        virtual bool IsConnected() const;

        virtual DataMap GetLatestData(VatCommand type) const;
        void SetLatestData(VatCommand type, const DataMap& data);
        virtual void ClearLatestData(VatCommand type);
        virtual bool IsValid(VatCommand type) const;
        virtual bool HasReceived(VatCommand type) const;

        void SetReceived(VatCommand type, bool received);
        void ClearReceived(VatCommand type);

        void StartProcessThread();
        void StopProcessThread();

    protected:
        VisionCom::VisionController m_ctrl;
        boost::mutex m_mutex;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

        void SendResultToSink(int requestId, const std::vector<std::string>& results);
        void PacketLoop();
    };
} // namespace DVH_VAT
