#include "stdafx.h"
#include <vector>   
#include <map>
#include <string>

#include "VisionCommunicationManager.h"
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
    VisionCommunicationManager::VisionCommunicationManager()
        : m_processRunning(false)
        , m_mainRunning(false)
        , m_resultSink(nullptr)
    {
    }

    VisionCommunicationManager::~VisionCommunicationManager()
    {
        StopProcessThread();
        Stop();
    }

    void VisionCommunicationManager::Start()
    {
        if (m_thread && m_thread->joinable())
        {
            return;
        }

        m_mainRunning = true;
        m_thread = std::make_unique<std::thread>(&VisionCommunicationManager::RunLoop, this);
    }

    void VisionCommunicationManager::Stop()
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

    void VisionCommunicationManager::InitializeRecvThread()
    {
        m_ctrl.StartReceiving();
    }

    void VisionCommunicationManager::RunLoop()
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

    void VisionCommunicationManager::Process()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    VC::Status VisionCommunicationManager::Initialize(const VisionConnectionConfig& config)
    {
        m_ctrl.Initialize(const_cast<char*>(config.address.c_str()), config.port, 0, config.timeoutMs);
        InitializeRecvThread();
        StartProcessThread();
        return VC::VisionOK;
    }

    void VisionCommunicationManager::Disconnect()
    {
        m_ctrl.Disconnect();
    }

    bool VisionCommunicationManager::IsConnected() const
    {
        return m_ctrl.IsConnected();
    }

    VisionCommunicationManager::DataMap VisionCommunicationManager::GetLatestData(VisionCommand type) const
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);

        auto it = m_latestData.find(type);
        if (it != m_latestData.end())
        {
            return it->second;
        }

        return DataMap();
    }

    void VisionCommunicationManager::SetLatestData(VisionCommand type, const DataMap& data)
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);
        m_latestData[type] = data;
        m_received[type] = true;
    }

    void VisionCommunicationManager::ClearLatestData(VisionCommand type)
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);
        m_latestData.erase(type);
        m_received.erase(type);
    }

    bool VisionCommunicationManager::IsValid(VisionCommand type) const
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

    bool VisionCommunicationManager::HasReceived(VisionCommand type) const
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);

        auto it = m_received.find(type);
        return (it != m_received.end()) ? it->second : false;
    }

    void VisionCommunicationManager::SetReceived(VisionCommand type, bool received)
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);
        m_received[type] = received;
    }

    void VisionCommunicationManager::ClearReceived(VisionCommand type)
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);
        m_received.erase(type);
    }

    void VisionCommunicationManager::StartProcessThread()
    {
        if (m_processThread && m_processThread->joinable())
        {
            return;
        }

        m_processRunning = true;
        m_processThread = std::make_unique<std::thread>(&VisionCommunicationManager::PacketLoop, this);
    }

    void VisionCommunicationManager::StopProcessThread()
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

    void VisionCommunicationManager::PacketLoop()
    {
        while (m_processRunning)
        {
            m_ctrl.PacketThread();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void VisionCommunicationManager::SetResultSink(IResultSink* sink)
    {
        std::lock_guard<std::mutex> lk(m_dataMutex);
        m_resultSink = sink;
    }

    void VisionCommunicationManager::SendResultToSink(int requestId, const std::vector<std::string>& results)
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


