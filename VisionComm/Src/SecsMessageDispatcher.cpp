#include "stdafx.h"

#include "SecsMessageDispatcher.h"
#include "IScheduler.h"
#include "TaskExecutor.h"
#include "Logger.h"

#include <mutex>
#include <functional>
#include <map>
#include <utility>
#include <vector>

namespace VisionComm
{
    struct SecsMessageDispatcher::Impl
    {
        using SFKey = std::pair<int, int>;
        std::map<SFKey, PacketHandler> m_map;
        std::mutex m_mapMutex;
        std::shared_ptr<IScheduler> m_scheduler;

        Impl() : m_scheduler() {}
    };

    SecsMessageDispatcher::SecsMessageDispatcher() 
        : m_pImpl(std::make_unique<Impl>()) {}
        
    SecsMessageDispatcher::~SecsMessageDispatcher() = default; // .cpp 내에서 명시(Impl 정의 이후)

    void SecsMessageDispatcher::SetScheduler(std::shared_ptr<IScheduler> scheduler)
    {
        if (!m_pImpl) return;
        std::lock_guard<std::mutex> lock(m_pImpl->m_mapMutex);
        m_pImpl->m_scheduler = scheduler;
    }

    // PacketHandler uses std::function (or similar callable)
    void SecsMessageDispatcher::RegisterHandler(int s, int f, PacketHandler handler)
    {
        if (!m_pImpl) return;
        std::lock_guard<std::mutex> lock(m_pImpl->m_mapMutex);
        m_pImpl->m_map[std::make_pair(s, f)] = std::move(handler);
    }

    void SecsMessageDispatcher::RegisterHandler(const VisionProtocolId& protocolId, PacketHandler handler)
    {
        RegisterHandler(static_cast<int>(protocolId.stream), static_cast<int>(protocolId.function), std::move(handler));
    }

    void SecsMessageDispatcher::UnregisterHandler(int s, int f)
    {
        if (!m_pImpl) return;
        std::lock_guard<std::mutex> lock(m_pImpl->m_mapMutex);
        m_pImpl->m_map.erase(std::make_pair(s, f));
    }

    void SecsMessageDispatcher::UnregisterHandler(const VisionProtocolId& protocolId)
    {
        UnregisterHandler(static_cast<int>(protocolId.stream), static_cast<int>(protocolId.function));
    }

    bool SecsMessageDispatcher::HasHandler(int s, int f)
    {
        if (!m_pImpl) return false;
        std::lock_guard<std::mutex> lock(m_pImpl->m_mapMutex);
        return m_pImpl->m_map.find(std::make_pair(s, f)) != m_pImpl->m_map.end();
    }

    bool SecsMessageDispatcher::HasHandler(const VisionProtocolId& protocolId)
    {
        return HasHandler(static_cast<int>(protocolId.stream), static_cast<int>(protocolId.function));
    }

    void SecsMessageDispatcher::OnReceive(int s, int f, ByteArray&& incoming, int serverIndex)
    {
        if (!m_pImpl) return;

        PacketHandler handler;
        std::shared_ptr<IScheduler> scheduler;
        {
            std::lock_guard<std::mutex> lock(m_pImpl->m_mapMutex);
            auto it = m_pImpl->m_map.find(std::make_pair(s, f));
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
                    // Move incoming into the scheduled task to avoid copies
                    SchedulerTask task = [handler, s, f, incoming = std::move(incoming), serverIndex]() mutable {
                        handler(s, f, std::move(incoming), serverIndex);
                    };
                    scheduler->Schedule(task, 0);
                }
                else
                {
                    handler(s, f, std::move(incoming), serverIndex);
                }
            }
            catch (...)
            {
            }
        }
    }

    void SecsMessageDispatcher::Dispatch(const SECSPacketHeader& header, ByteArray&& body)
    {
        OnReceive(header.nS, header.nF, std::move(body), 0);
    }

} // namespace VisionCommm

