#pragma once

#include <memory>
#include <vector>
#include <mutex> 
#include <map>
#include <thread>
#include <atomic>

#include "VMF_API.h"
#include "IVisionProcessor.h"
#include "IResultSink.h"
#include "Controller.h"

namespace VMF
{
    class VMF_API VisionProcessorBase : public IVisionEventHandler
    {
    public:
        using DataMap = StringMap;

        VisionProcessorBase();
        ~VisionProcessorBase() override;

        void Start();
        void Stop();
        
        // 상속된 가상 함수
        void InitializeRecvThread() override;

        void RunLoop();
        virtual void Process();

        VC::Status Initialize(const VisionConnectionConfig& config) override;
        void Disconnect() override;
        bool IsConnected() const override;

        DataMap GetLatestData(VatCommand type) const override;
        void SetLatestData(VatCommand type, const DataMap& data);
        void ClearLatestData(VatCommand type) override;
        bool IsValid(VatCommand type) const override;
        bool HasReceived(VatCommand type) const override;

        void SetReceived(VatCommand type, bool received);
        void ClearReceived(VatCommand type);

        /// <summary>
        /// 비전 프로세서의 실행 결과를 수신할 IResultSink를 등록합니다.
        /// </summary>
        void SetResultSink(IResultSink* sink);

        void StartProcessThread();
        void StopProcessThread();

    protected:
        VC::Controller m_ctrl;
        std::mutex m_mutex;

    private:
        std::map<VatCommand, bool> m_received;
        std::map<VatCommand, DataMap> m_latestData;

mutable std::mutex m_dataMutex;
        std::atomic<bool> m_processRunning;
        std::atomic<bool> m_mainRunning;
        
        std::unique_ptr<std::thread> m_processThread;
        std::unique_ptr<std::thread> m_thread;

        IResultSink* m_resultSink;

        void SendResultToSink(int requestId, const std::vector<std::string>& results);
        void PacketLoop();
    };
} // namespace VMF

