#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <map>
#include <thread>
#include <atomic>

#include "VMF_API.h"
#include "IVisionClient.h"
#include "IResultSink.h"
#include "VisionComm\Controller.h"

namespace VMF
{
    class VMF_API VisionClientManager : public IVisionClient
    {
    public:
        using DataMap = StringMap;

        VisionClientManager();
        ~VisionClientManager() override;

        void Start();
        void Stop();
        
        // 상속된 가상 함수
        void InitializeRecvThread() override;

        void RunLoop();
        virtual void Process();

        VC::Status Initialize(const VisionConnectionConfig& config) override;
        void Disconnect() override;
        bool IsConnected() const override;

        DataMap GetLatestData(VisionCommand type) const override;
        void SetLatestData(VisionCommand type, const DataMap& data);
        void ClearLatestData(VisionCommand type) override;
        bool IsValid(VisionCommand type) const override;
        bool HasReceived(VisionCommand type) const override;

        void SetReceived(VisionCommand type, bool received);
        void ClearReceived(VisionCommand type);

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
        std::map<VisionCommand, bool> m_received;
        std::map<VisionCommand, DataMap> m_latestData;

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


