#include "stdafx.h"
#include <vector>   
#include <map>
#include <string>

#include "VisionProcessorBase.h"
#include "SecsMessageDispatcher.h"
#include "SECSPacket.h"
#include "IResultSink.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <memory>
#include <algorithm>

namespace VMF
{
    VisionProcessorBase::VisionProcessorBase()
        : m_processRunning(false)
        , m_mainRunning(false)
        , m_resultSink(nullptr)
    {
    }

    VisionProcessorBase::~VisionProcessorBase()
    {
        StopProcessThread();
        Stop();
    }

    void VisionProcessorBase::Start()
    {
        if (m_thread && m_thread->joinable())
        {
            return;
        }

        m_mainRunning = true;
        m_thread = std::make_unique<std::thread>(&VisionProcessorBase::RunLoop, this);
    }

    void VisionProcessorBase::Stop()
    {
        m_mainRunning = false;

        if (m_thread && m_thread->joinable())
        {
            try
            {
                m_thread->join();
            }
            catch (...)
            {
            }
        }

        m_thread.reset();
    }

    void VisionProcessorBase::InitializeRecvThread()
    {
        // 공유 Controller를 사용하는 경우, 수신 스레드는 ConnectionManager가 관리하므로
        // 여기서는 시작하지 않음 (이미 시작되어 있음)
        if (m_sharedCtrl)
        {
            // sharedCtrl은 이미 ConnectionManager에서 StartReceiving() 호출 완료
            return;
        }
        m_ctrl.StartReceiving();
    }

    void VisionProcessorBase::RunLoop()
    {
        try
        {
            while (m_mainRunning)
            {
                Process();
            }
        }
        catch (...)
        {
        }
    }

    void VisionProcessorBase::Process()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    VC::Status VisionProcessorBase::Initialize(const VisionConnectionConfig& config)
    {
        m_ctrl.Initialize(const_cast<char*>(config.address.c_str()), config.port, 0, config.timeoutMs);
        InitializeRecvThread();
        StartProcessThread();
        return VC::VisionOK;
    }

    VC::Status VisionProcessorBase::InitializeWithSharedController(
        std::shared_ptr<VC::Controller> sharedCtrl,
        const VisionConnectionConfig& config)
    {
        if (!sharedCtrl || !sharedCtrl->IsConnected())
        {
            return VC::VisionConnectionFailed;
        }

        // 공유 Controller 저장
        m_sharedCtrl = sharedCtrl;

        // m_ctrl을 sharedCtrl의 복사본으로 설정 (같은 Impl 공유)
        // Controller의 m_pImpl이 shared_ptr이므로, 복사 시 같은 Impl 인스턴스를 가리킴
        m_ctrl = *sharedCtrl;

        // sharedCtrl은 이미 ConnectionManager에서 StartReceiving()이 호출된 상태
        // InitializeRecvThread()는 m_sharedCtrl이 있으면 중복 호출하지 않음

        // PacketLoop 스레드 시작 (공유 큐에서 패킷을 가져와 처리)
        StartProcessThread();

        return VC::VisionOK;
    }

    void VisionProcessorBase::Disconnect()
    {
        if (m_sharedCtrl)
        {
            // 공유 Controller는 ConnectionManager가 관리하므로 disconnect하지 않음
            // 대신 PacketLoop만 중단
            StopProcessThread();
            return;
        }
        m_ctrl.Disconnect();
    }

    bool VisionProcessorBase::IsConnected() const
    {
        if (m_sharedCtrl)
        {
            return m_sharedCtrl->IsConnected();
        }
        return m_ctrl.IsConnected();
    }

    VisionProcessorBase::DataMap VisionProcessorBase::GetLatestData(VisionCommand type) const
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);

        auto it = m_latestData.find(type);
        if (it != m_latestData.end())
        {
            return it->second;
        }

        return DataMap();
    }

    void VisionProcessorBase::SetLatestData(VisionCommand type, const DataMap& data)
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);
        m_latestData[type] = data;
        m_received[type] = true;
    }

    void VisionProcessorBase::ClearLatestData(VisionCommand type)
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);
        m_latestData.erase(type);
        m_received.erase(type);
    }

    bool VisionProcessorBase::IsValid(VisionCommand type) const
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);

        auto receivedIt = m_received.find(type);
        if (receivedIt == m_received.end() || !receivedIt->second)
        {
            return false;
        }

        auto dataIt = m_latestData.find(type);
        return (dataIt != m_latestData.end() && !dataIt->second.empty());
    }

    bool VisionProcessorBase::HasReceived(VisionCommand type) const
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);

        auto it = m_received.find(type);
        return (it != m_received.end()) ? it->second : false;
    }

    void VisionProcessorBase::SetReceived(VisionCommand type, bool received)
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);
        m_received[type] = received;
    }

    void VisionProcessorBase::ClearReceived(VisionCommand type)
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);
        m_received.erase(type);
    }

    void VisionProcessorBase::StartProcessThread()
    {
        if (m_processThread && m_processThread->joinable())
        {
            return;
        }

        m_processRunning = true;
        m_processThread = std::make_unique<std::thread>(&VisionProcessorBase::PacketLoop, this);
    }

    void VisionProcessorBase::StopProcessThread()
    {
        m_processRunning = false;

        if (m_processThread && m_processThread->joinable())
        {
            try
            {
                m_processThread->join();
            }
            catch (...)
            {
            }

            m_processThread.reset();
        }
    }

    void VisionProcessorBase::PacketLoop()
    {
        // 공유 Controller 사용 시, 해당 Controller의 PacketThread()를 호출
        // (m_ctrl은 sharedCtrl의 복사본이므로 같은 Impl을 공유)
        while (m_processRunning)
        {
            m_ctrl.PacketThread();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void VisionProcessorBase::SetResultSink(IResultSink* sink)
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);
        m_resultSink = sink;
    }

    void VisionProcessorBase::SendResultToSink(int requestId, const std::vector<std::string>& results)
    {
        IResultSink* sink = nullptr;
        {
            std::lock_guard<std::mutex> lk(m_dataMutex);
            sink = m_resultSink;
        }

        if (sink)
        {
            sink->NotifyVisionResult(requestId, results);
        }
    }
}


