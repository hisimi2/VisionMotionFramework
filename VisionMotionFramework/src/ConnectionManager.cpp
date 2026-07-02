#include "stdafx.h"
#include "ConnectionManager.h"
#include "Controller.h"

namespace VMF
{
    ConnectionManager& ConnectionManager::GetInstance()
    {
        static ConnectionManager instance;
        return instance;
    }

    ConnectionManager::~ConnectionManager()
    {
        DisconnectAll();
    }

    std::shared_ptr<VC::Controller> ConnectionManager::GetOrCreateConnection(
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

    void ConnectionManager::ReleaseConnection(const std::string& key)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connections.erase(key);
    }

    void ConnectionManager::DisconnectAll()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connections.clear();
    }

    std::string ConnectionManager::MakeConnectionKey(const std::string& ip, int port)
    {
        return ip + ":" + std::to_string(port);
    }
} // namespace VMF