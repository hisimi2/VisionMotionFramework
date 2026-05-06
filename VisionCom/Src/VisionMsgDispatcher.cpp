// [필수] 가장 먼저 포함
#include "stdafx.h"

#include "VisionMsgDispatcher.h"
// [FIX] SchedulerTask 타입 인식을 위해 명시적 포함
#include "IScheduler.h" 
#include "WorkerScheduler.h"
#include "VisionLogger.h"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <map>
#include <utility>

namespace VisionCom
{
    struct VisionMsgDispatcher::Impl
    {
        typedef std::pair<int, int> SFKey;
        std::map<SFKey, PacketHandler> m_map;
        boost::mutex m_mapMutex;

        std::shared_ptr<IScheduler> m_scheduler;

        Impl() : m_scheduler() {}
    };

    VisionMsgDispatcher::VisionMsgDispatcher()
        : m_pImpl(new Impl())
    {
    }

    VisionMsgDispatcher::~VisionMsgDispatcher()
    {
        if (m_pImpl)
        {
            delete m_pImpl;
            m_pImpl = NULL;
        }
    }

    void VisionMsgDispatcher::SetScheduler(std::shared_ptr<IScheduler> scheduler)
    {
        if (!m_pImpl) return;
        boost::mutex::scoped_lock lock(m_pImpl->m_mapMutex);
        m_pImpl->m_scheduler = scheduler;
    }

    // PacketHandler uses boost::function effectively
    void VisionMsgDispatcher::RegisterHandler(int s, int f, PacketHandler handler)
    {
        if (!m_pImpl) return;
        boost::mutex::scoped_lock lock(m_pImpl->m_mapMutex);
        m_pImpl->m_map[std::make_pair(s, f)] = handler;
    }

    void VisionMsgDispatcher::UnregisterHandler(int s, int f)
    {
        if (!m_pImpl) return;
        boost::mutex::scoped_lock lock(m_pImpl->m_mapMutex);
        m_pImpl->m_map.erase(std::make_pair(s, f));
    }

    bool VisionMsgDispatcher::HasHandler(int s, int f)
    {
        if (!m_pImpl) return false;
        boost::mutex::scoped_lock lock(m_pImpl->m_mapMutex);
        return m_pImpl->m_map.find(std::make_pair(s, f)) != m_pImpl->m_map.end();
    }

    void VisionMsgDispatcher::OnReceive(int s, int f, const std::vector<uint8_t>& body, int serverIndex)
    {
        if (!m_pImpl) return;

        PacketHandler handler;
        std::shared_ptr<IScheduler> scheduler;
        {
            boost::mutex::scoped_lock lock(m_pImpl->m_mapMutex);
            std::map<Impl::SFKey, PacketHandler>::iterator it = m_pImpl->m_map.find(std::make_pair(s, f));
            if (it != m_pImpl->m_map.end())
            {
                handler = it->second;
            }
            scheduler = m_pImpl->m_scheduler;
        }

        if (handler)
        {
            try
            {
                if (scheduler)
                {
                    SchedulerTask task;
                    task = boost::bind(handler, s, f, body, serverIndex);
                    scheduler->Schedule(task, 0);
                }
                else
                {
                    handler(s, f, body, serverIndex);
                }
            }
            catch (...)
            {
            }
        }
    }

    void VisionMsgDispatcher::Dispatch(const SECSPacketHeader& header, const ByteArray& body)
    {
        OnReceive(header.nS, header.nF, body, 0);
    }

} // namespace VisionCom
