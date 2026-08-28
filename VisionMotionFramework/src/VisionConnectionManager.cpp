#include "stdafx.h"
#include "VisionConnectionManager.h"
#include "VisionComm\Controller.h"

namespace VMF
{
    VisionConnectionManager& VisionConnectionManager::GetInstance()
    {
        static VisionConnectionManager instance;
        return instance;
    }

    VisionConnectionManager::~VisionConnectionManager()
    {
        DisconnectAll();
    }

    std::shared_ptr<VC::Controller> VisionConnectionManager::GetOrCreateConnection(
        const std::string& ip, int port, int timeoutMs)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        std::string key = MakeConnectionKey(ip, port);
        auto it = m_connections.find(key);

        if (it != m_connections.end())
        {
            auto ctrl = it->second.lock();
            if (ctrl)
                return ctrl;
            else
                m_connections.erase(it); // 만료된 weak_ptr 제거
        }

        // 새 Controller 생성
        auto ctrl = std::make_shared<VC::Controller>();
        m_connections[key] = ctrl;
        return ctrl;
    }

    void VisionConnectionManager::ReleaseConnection(const std::string& key)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connections.erase(key);
    }

    void VisionConnectionManager::DisconnectAll()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connections.clear();
    }

    std::string VisionConnectionManager::MakeConnectionKey(const std::string& ip, int port)
    {
        return ip + ":" + std::to_string(port);
    }
} // namespace VMF
