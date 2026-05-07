// [필수] 가장 먼저 포함
#include "stdafx.h"

#include "VisionMsgDispatcher.h"
#include "IScheduler.h"
#include "TaskExecutor.h"
#include "VisionLogger.h"

#include <mutex>
#include <functional>
#include <map>
#include <utility>
#include <vector>

namespace VisionCom
{
    struct VisionMsgDispatcher::Impl
    {
        using SFKey = std::pair<int, int>;
        std::map<SFKey, PacketHandler> m_map;
        std::mutex m_mapMutex;
        std::shared_ptr<IScheduler> m_scheduler;

        Impl() : m_scheduler() {}
    };

    VisionMsgDispatcher::VisionMsgDispatcher() 
        : m_pImpl(std::make_unique<Impl>()) {}
        
    VisionMsgDispatcher::~VisionMsgDispatcher() = default; // .cpp 내에서 명시(Impl 정의 이후)

    void VisionMsgDispatcher::SetScheduler(std::shared_ptr<IScheduler> scheduler)
    {
        if (!m_pImpl) return;
        std::lock_guard<std::mutex> lock(m_pImpl->m_mapMutex);
        m_pImpl->m_scheduler = scheduler;
    }

    // PacketHandler uses std::function (or similar callable)
    void VisionMsgDispatcher::RegisterHandler(int s, int f, PacketHandler handler)
    {
        if (!m_pImpl) return;
        std::lock_guard<std::mutex> lock(m_pImpl->m_mapMutex);
        m_pImpl->m_map[std::make_pair(s, f)] = std::move(handler);
    }

    void VisionMsgDispatcher::RegisterHandler(const VisionProtocolId& protocolId, PacketHandler handler)
    {
        RegisterHandler(static_cast<int>(protocolId.stream), static_cast<int>(protocolId.function), std::move(handler));
    }

    void VisionMsgDispatcher::UnregisterHandler(int s, int f)
    {
        if (!m_pImpl) return;
        std::lock_guard<std::mutex> lock(m_pImpl->m_mapMutex);
        m_pImpl->m_map.erase(std::make_pair(s, f));
    }

    void VisionMsgDispatcher::UnregisterHandler(const VisionProtocolId& protocolId)
    {
        UnregisterHandler(static_cast<int>(protocolId.stream), static_cast<int>(protocolId.function));
    }

    bool VisionMsgDispatcher::HasHandler(int s, int f)
    {
        if (!m_pImpl) return false;
        std::lock_guard<std::mutex> lock(m_pImpl->m_mapMutex);
        return m_pImpl->m_map.find(std::make_pair(s, f)) != m_pImpl->m_map.end();
    }

    bool VisionMsgDispatcher::HasHandler(const VisionProtocolId& protocolId)
    {
        return HasHandler(static_cast<int>(protocolId.stream), static_cast<int>(protocolId.function));
    }

    void VisionMsgDispatcher::OnReceive(int s, int f, const std::vector<uint8_t>& body, int serverIndex)
    {
        if (!m_pImpl) return;

        PacketHandler handler;
        std::shared_ptr<IScheduler> scheduler;
        {
            std::lock_guard<std::mutex> lock(m_pImpl->m_mapMutex);
            // C++14: auto를 활용한 타입 추론으로 간결하게 작성
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
                    // boost::bind 대신 C++11 람다 식 사용
                    SchedulerTask task = [handler, s, f, body, serverIndex]() {
                        handler(s, f, body, serverIndex);
                    };
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
