// [필수] 가장 먼저 포함
#include "stdafx.h"

#include "VisionMsgDispatcher.h"
// [FIX] SchedulerTask 타입 인식을 위해 명시적 포함
#include "IScheduler.h" 
#include "WorkerScheduler.h"
#include "VisionLogger.h"

// Boost 대신 C++ 표준 라이브러리 사용
#include <mutex>
#include <functional>
#include <map>
#include <utility>
#include <vector>

namespace VisionCom
{
    struct VisionMsgDispatcher::Impl
    {
        // C++11/14: typedef 대신 using 사용 권장
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
        // boost::mutex::scoped_lock -> std::lock_guard
        std::lock_guard<std::mutex> lock(m_pImpl->m_mapMutex);
        m_pImpl->m_scheduler = scheduler;
    }

    // PacketHandler uses std::function (or similar callable)
    void VisionMsgDispatcher::RegisterHandler(int s, int f, PacketHandler handler)
    {
        if (!m_pImpl) return;
        std::lock_guard<std::mutex> lock(m_pImpl->m_mapMutex);
        m_pImpl->m_map[std::make_pair(s, f)] = handler;
    }

    void VisionMsgDispatcher::UnregisterHandler(int s, int f)
    {
        if (!m_pImpl) return;
        std::lock_guard<std::mutex> lock(m_pImpl->m_mapMutex);
        m_pImpl->m_map.erase(std::make_pair(s, f));
    }

    bool VisionMsgDispatcher::HasHandler(int s, int f)
    {
        if (!m_pImpl) return false;
        std::lock_guard<std::mutex> lock(m_pImpl->m_mapMutex);
        return m_pImpl->m_map.find(std::make_pair(s, f)) != m_pImpl->m_map.end();
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
