#include "stdafx.h"
#include <vector>   
#include <map>
#include <string>

#include "VisionClientManager.h"
#include "VisionComm\SecsMessageDispatcher.h"
#include "VisionComm\SECSPacket.h"
#include "IResultSink.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <memory>
#include <algorithm>

namespace VMF
{
    VisionClientManager::VisionClientManager()
        : m_processRunning(false)
        , m_mainRunning(false)
        , m_resultSink(nullptr)
    {
    }

    VisionClientManager::~VisionClientManager()
    {
        StopProcessThread();
        Stop();
    }

    void VisionClientManager::Start()
    {
        if (m_thread && m_thread->joinable())
        {
            return;
        }

        m_mainRunning = true;
        m_thread = std::make_unique<std::thread>(&VisionClientManager::RunLoop, this);
    }

    void VisionClientManager::Stop()
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

    void VisionClientManager::InitializeRecvThread()
    {
        m_ctrl.StartReceiving();
    }

    void VisionClientManager::RunLoop()
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

    void VisionClientManager::Process()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    VC::Status VisionClientManager::Initialize(const VisionConnectionConfig& config)
    {
        m_ctrl.Initialize(const_cast<char*>(config.address.c_str()), config.port, 0, config.timeoutMs);
        InitializeRecvThread();
        StartProcessThread();
        return VC::VisionOK;
    }

    void VisionClientManager::Disconnect()
    {
        m_ctrl.Disconnect();
    }

    bool VisionClientManager::IsConnected() const
    {
        return m_ctrl.IsConnected();
    }

    VisionClientManager::DataMap VisionClientManager::GetLatestData(VisionCommand type) const
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);

        auto it = m_latestData.find(type);
        if (it != m_latestData.end())
        {
            return it->second;
        }

        return DataMap();
    }

    void VisionClientManager::SetLatestData(VisionCommand type, const DataMap& data)
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);
        m_latestData[type] = data;
        m_received[type] = true;
    }

    void VisionClientManager::ClearLatestData(VisionCommand type)
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);
        m_latestData.erase(type);
        m_received.erase(type);
    }

    bool VisionClientManager::IsValid(VisionCommand type) const
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

    bool VisionClientManager::HasReceived(VisionCommand type) const
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);

        auto it = m_received.find(type);
        return (it != m_received.end()) ? it->second : false;
    }

    void VisionClientManager::SetReceived(VisionCommand type, bool received)
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);
        m_received[type] = received;
    }

    void VisionClientManager::ClearReceived(VisionCommand type)
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);
        m_received.erase(type);
    }

    void VisionClientManager::StartProcessThread()
    {
        if (m_processThread && m_processThread->joinable())
        {
            return;
        }

        m_processRunning = true;
        m_processThread = std::make_unique<std::thread>(&VisionClientManager::PacketLoop, this);
    }

    void VisionClientManager::StopProcessThread()
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

    void VisionClientManager::PacketLoop()
    {
        while (m_processRunning)
        {
            m_ctrl.PacketThread();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void VisionClientManager::SetResultSink(IResultSink* sink)
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);
        m_resultSink = sink;
    }

    void VisionClientManager::SendResultToSink(int requestId, const std::vector<std::string>& results)
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


