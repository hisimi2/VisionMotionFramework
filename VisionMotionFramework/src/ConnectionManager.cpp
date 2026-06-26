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

    std::string ConnectionManager::MakeConnectionKey(const std::string& ip, int port)
    {
        return ip + ":" + std::to_string(port);
    }

    std::shared_ptr<VC::Controller> ConnectionManager::GetOrCreateConnection(
        const std::string& ip, int port, int timeoutMs)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        std::string key = MakeConnectionKey(ip, port);

        // 1. 기존 연결이 있는지 확인
        auto it = m_connections.find(key);
        if (it != m_connections.end())
        {
            auto controller = it->second.lock();
            if (controller && controller->IsConnected())
            {
                // 기존 연결 재사용
                return controller;
            }
            // 연결이 끊어졌으면 맵에서 제거
            m_connections.erase(it);
        }

        // 2. 새 Controller 생성 및 연결
        auto controller = std::make_shared<VC::Controller>();
        controller->SetConnectionKey(key);

        if (!controller->Initialize(ip.c_str(), port, 0, timeoutMs))
        {
            // 연결 실패
            return nullptr;
        }

        // 3. 수신 시작 (최초 연결 시에만)
        controller->StartReceiving();

        // 4. 맵에 저장 (weak_ptr)
        m_connections[key] = controller;

        return controller;
    }

    void ConnectionManager::ReleaseConnection(const std::string& key)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_connections.find(key);
        if (it != m_connections.end())
        {
            if (auto controller = it->second.lock())
            {
                controller->StopReceiving();
                controller->Disconnect();
            }
            m_connections.erase(it);
        }
    }

    void ConnectionManager::DisconnectAll()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        for (auto it = m_connections.begin(); it != m_connections.end(); ++it)
        {
            if (auto controller = it->second.lock())
            {
                controller->StopReceiving();
                controller->Disconnect();
            }
        }
        m_connections.clear();
    }
} // namespace VMF
