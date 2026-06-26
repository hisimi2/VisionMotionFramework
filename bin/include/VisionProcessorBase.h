#pragma once

#include <memory>
#include <map>
#include <thread>
#include <atomic>

#include "VMF_API.h"
#include "IVisionProcessor.h"
#include "IResultSink.h"
#include "Controller.h"

namespace VMF
{
    class VMF_API VisionProcessorBase : public IVisionProcessor
    {
    public:
        using DataMap = StringMap;

        VisionProcessorBase();
        ~VisionProcessorBase() override;

        void Start();
        void Stop();

        void InitializeRecvThread() override;

        void RunLoop();
        virtual void Process();

        VC::Status Initialize(const VisionConnectionConfig& config) override;
        void Disconnect() override;
        bool IsConnected() const override;

        VC::Status InitializeWithSharedController(
            std::shared_ptr<VC::Controller> sharedCtrl,
            const VisionConnectionConfig& config) override;

        DataMap GetLatestData(VisionCommand type) const override;
        void SetLatestData(VisionCommand type, const DataMap& data);
        void ClearLatestData(VisionCommand type) override;
        bool IsValid(VisionCommand type) const override;
        bool HasReceived(VisionCommand type) const override;

        void SetReceived(VisionCommand type, bool received);
        void ClearReceived(VisionCommand type);

        void SetResultSink(IResultSink* sink);

        void StartProcessThread();
        void StopProcessThread();

        VC::Controller& GetControllerRef() { return m_ctrl; }
        std::shared_ptr<VC::Controller> GetSharedController() const { return m_sharedCtrl; }

    protected:
        VC::Controller  m_ctrl;
        std::shared_ptr<VC::Controller> m_sharedCtrl;
        std::mutex      m_mutex;

    private:
        std::map<VisionCommand, bool>       m_received;
        std::map<VisionCommand, DataMap>    m_latestData;
        mutable std::mutex                  m_dataMutex;

        std::atomic<bool> m_processRunning;
        std::atomic<bool> m_mainRunning;

        std::unique_ptr<std::thread> m_processThread;
        std::unique_ptr<std::thread> m_thread;

        IResultSink* m_resultSink;

        void SendResultToSink(int requestId, const std::vector<std::string>& results);
        void PacketLoop();
    };
} // namespace VMF

