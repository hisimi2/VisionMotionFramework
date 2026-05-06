#include "stdafx.h"
#include <vector>
#include <map>
#include <string>

#include "VisionProcessorBase.h"
#include "VisionMsgDispatcher.h"
#include "SECSIPacket.h"
#include "IResultSink.h"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/atomic.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include <algorithm>

namespace DVH_VAT
{
    struct VisionProcessorBase::Impl
    {
        Impl()
            : m_processRunning(false)
        {
        }

        std::map<VatCommand, bool> m_received;
        std::map<VatCommand, VisionProcessorBase::DataMap> m_latestData;

        boost::mutex m_dataMutex;
        boost::atomic<bool> m_processRunning;
        boost::shared_ptr<boost::thread> m_processThread;
        boost::shared_ptr<boost::thread> m_thread;
    };

    VisionProcessorBase::VisionProcessorBase()
        : m_impl(new Impl())
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

        m_impl->m_thread = boost::make_shared<boost::thread>(
            boost::bind(&VisionProcessorBase::RunLoop, this));
    }

    void VisionProcessorBase::Stop()
    {
        if (!m_impl->m_thread)
        {
            return;
        }

        try
        {
            m_impl->m_thread->interrupt();
            m_impl->m_thread->join();
        }
        catch (...)
        {
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
            while (true)
            {
                boost::this_thread::interruption_point();
                Process();
            }
        }
        catch (...)
        {
        }
    }

    void VisionProcessorBase::Process()
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    }

    VisionCom::VisionStatus VisionProcessorBase::Initialize(const VisionConnectionConfig& config)
    {
        m_ctrl.Initialize(const_cast<char*>(config.address.c_str()), config.port, 0, config.timeoutMs);
        InitializeRecvThread();
        StartProcessThread();
        return VisionCom::VisionOK;
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
        boost::mutex::scoped_lock lk(m_impl->m_dataMutex);

        std::map<VatCommand, DataMap>::const_iterator it = m_impl->m_latestData.find(type);
        if (it != m_impl->m_latestData.end())
        {
            return it->second;
        }

        return DataMap();
    }

    void VisionProcessorBase::SetLatestData(VatCommand type, const DataMap& data)
    {
        boost::mutex::scoped_lock lk(m_impl->m_dataMutex);
        m_impl->m_latestData[type] = data;
        m_impl->m_received[type] = true;
    }

    void VisionProcessorBase::ClearLatestData(VatCommand type)
    {
        boost::mutex::scoped_lock lk(m_impl->m_dataMutex);
        m_impl->m_latestData.erase(type);
        m_impl->m_received.erase(type);
    }

    bool VisionProcessorBase::IsValid(VatCommand type) const
    {
        boost::mutex::scoped_lock lk(m_impl->m_dataMutex);

        std::map<VatCommand, bool>::const_iterator receivedIt = m_impl->m_received.find(type);
        if (receivedIt == m_impl->m_received.end() || !receivedIt->second)
        {
            return false;
        }

        std::map<VatCommand, DataMap>::const_iterator dataIt = m_impl->m_latestData.find(type);
        return (dataIt != m_impl->m_latestData.end() && !dataIt->second.empty());
    }

    bool VisionProcessorBase::HasReceived(VatCommand type) const
    {
        boost::mutex::scoped_lock lk(m_impl->m_dataMutex);

        std::map<VatCommand, bool>::const_iterator it = m_impl->m_received.find(type);
        return (it != m_impl->m_received.end()) ? it->second : false;
    }

    void VisionProcessorBase::SetReceived(VatCommand type, bool received)
    {
        boost::mutex::scoped_lock lk(m_impl->m_dataMutex);
        m_impl->m_received[type] = received;
    }

    void VisionProcessorBase::ClearReceived(VatCommand type)
    {
        boost::mutex::scoped_lock lk(m_impl->m_dataMutex);
        m_impl->m_received.erase(type);
    }

    void VisionProcessorBase::StartProcessThread()
    {
        if (m_impl->m_processThread && m_impl->m_processThread->joinable())
        {
            return;
        }

        m_impl->m_processRunning = true;
        m_impl->m_processThread = boost::make_shared<boost::thread>(
            boost::bind(&VisionProcessorBase::PacketLoop, this));
    }

    void VisionProcessorBase::StopProcessThread()
    {
        m_impl->m_processRunning = false;

        if (m_impl->m_processThread)
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
            boost::this_thread::sleep(boost::posix_time::milliseconds(1));
        }
    }

    void VisionProcessorBase::SendResultToSink(int requestId, const std::vector<std::string>& results)
    {
        (void)requestId;
        (void)results;
    }
}

