#include "stdafx.h"
#include <vector>   
#include <map>
#include <string>

#include "VisionProcessorBase.h"
#include "VisionMsgDispatcher.h"
#include "SECSIPacket.h"
#include "IResultSink.h"

// Boost 대신 C++ 표준 라이브러리 사용
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <memory>
#include <algorithm>

namespace DVH_VAT
{
    struct VisionProcessorBase::Impl
    {
        Impl()
            : m_processRunning(false), m_mainRunning(false)
        {
        }

        std::map<VatCommand, bool> m_received;
        std::map<VatCommand, VisionProcessorBase::DataMap> m_latestData;

        std::mutex m_dataMutex; // boost::mutex 대체
        std::atomic<bool> m_processRunning;
        std::atomic<bool> m_mainRunning; // 스레드 인터럽트 대체를 위한 실행 플래그
        
        // boost::shared_ptr<boost::thread> 대신 std::unique_ptr<std::thread> 사용 (단일 소유 제어)
        std::unique_ptr<std::thread> m_processThread;
        std::unique_ptr<std::thread> m_thread;
    };

    VisionProcessorBase::VisionProcessorBase()
        : m_impl(std::make_unique<Impl>()) // C++14 std::make_unique 적용
    {
    }

    VisionProcessorBase::~VisionProcessorBase()
    {
        StopProcessThread();
        Stop();
        m_impl.reset();
    }

    void VisionProcessorBase::Start()
    {
        if (m_impl->m_thread && m_impl->m_thread->joinable())
        {
            return;
        }

        m_impl->m_mainRunning = true;
        // boost::bind 대신 람다식 또는 멤버 함수 포인터 직접 전달
        m_impl->m_thread = std::make_unique<std::thread>(&VisionProcessorBase::RunLoop, this);
    }

    void VisionProcessorBase::Stop()
    {
        m_impl->m_mainRunning = false; // 안전한 종료 신호 전달

        if (m_impl->m_thread && m_impl->m_thread->joinable())
        {
            try
            {
                m_impl->m_thread->join();
            }
            catch (...)
            {
            }
        }

        m_impl->m_thread.reset();
    }

    void VisionProcessorBase::InitializeRecvThread()
    {
        m_ctrl.StartReceiving();
    }

    void VisionProcessorBase::RunLoop()
    {
        try
        {
            while (m_impl->m_mainRunning) // interruption_point 대신 원자적 플래그 확인
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
        // boost::this_thread -> std::this_thread
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    VisionComm::VisionStatus VisionProcessorBase::Initialize(const VisionConnectionConfig& config)
    {
        m_ctrl.Initialize(const_cast<char*>(config.address.c_str()), config.port, 0, config.timeoutMs);
        InitializeRecvThread();
        StartProcessThread();
        return VisionComm::VisionOK;
    }

    void VisionProcessorBase::Disconnect()
    {
        m_ctrl.Disconnect();
    }

    bool VisionProcessorBase::IsConnected() const
    {
        return m_ctrl.IsConnected();
    }

    VisionProcessorBase::DataMap VisionProcessorBase::GetLatestData(VatCommand type) const
    {
        // boost::mutex::scoped_lock -> std::lock_guard
        std::lock_guard<std::mutex> lk(m_impl->m_dataMutex);

        // C++11 auto 타입 추론 적용
        auto it = m_impl->m_latestData.find(type);
        if (it != m_impl->m_latestData.end())
        {
            return it->second;
        }

        return DataMap();
    }

    void VisionProcessorBase::SetLatestData(VatCommand type, const DataMap& data)
    {
        std::lock_guard<std::mutex> lk(m_impl->m_dataMutex);
        m_impl->m_latestData[type] = data;
        m_impl->m_received[type] = true;
    }

    void VisionProcessorBase::ClearLatestData(VatCommand type)
    {
        std::lock_guard<std::mutex> lk(m_impl->m_dataMutex);
        m_impl->m_latestData.erase(type);
        m_impl->m_received.erase(type);
    }

    bool VisionProcessorBase::IsValid(VatCommand type) const
    {
        std::lock_guard<std::mutex> lk(m_impl->m_dataMutex);

        auto receivedIt = m_impl->m_received.find(type);
        if (receivedIt == m_impl->m_received.end() || !receivedIt->second)
        {
            return false;
        }

        auto dataIt = m_impl->m_latestData.find(type);
        return (dataIt != m_impl->m_latestData.end() && !dataIt->second.empty());
    }

    bool VisionProcessorBase::HasReceived(VatCommand type) const
    {
        std::lock_guard<std::mutex> lk(m_impl->m_dataMutex);

        auto it = m_impl->m_received.find(type);
        return (it != m_impl->m_received.end()) ? it->second : false;
    }

    void VisionProcessorBase::SetReceived(VatCommand type, bool received)
    {
        std::lock_guard<std::mutex> lk(m_impl->m_dataMutex);
        m_impl->m_received[type] = received;
    }

    void VisionProcessorBase::ClearReceived(VatCommand type)
    {
        std::lock_guard<std::mutex> lk(m_impl->m_dataMutex);
        m_impl->m_received.erase(type);
    }

    void VisionProcessorBase::StartProcessThread()
    {
        if (m_impl->m_processThread && m_impl->m_processThread->joinable())
        {
            return;
        }

        m_impl->m_processRunning = true;
        m_impl->m_processThread = std::make_unique<std::thread>(&VisionProcessorBase::PacketLoop, this);
    }

    void VisionProcessorBase::StopProcessThread()
    {
        m_impl->m_processRunning = false;

        if (m_impl->m_processThread && m_impl->m_processThread->joinable())
        {
            try
            {
                m_impl->m_processThread->join();
            }
            catch (...)
            {
            }

            m_impl->m_processThread.reset();
        }
    }

    void VisionProcessorBase::PacketLoop()
    {
        while (m_impl->m_processRunning)
        {
            m_ctrl.PacketThread();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void VisionProcessorBase::SendResultToSink(int requestId, const std::vector<std::string>& results)
    {
        (void)requestId;
        (void)results;
    }
}


