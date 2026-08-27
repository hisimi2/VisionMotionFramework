#pragma once

#include "VMF_API.h"
#include "VisionComm\Controller.h"
#include <memory>
#include <string>
#include <map>
#include <mutex>

namespace VMF
{
    /// <summary>
    /// Vision 서버 연결을 중앙 관리하는 싱글톤 클래스.
    /// 동일 서버(IP:Port)에 대해서는 단일 Socket 연결을 공유하고,
    /// 다른 서버에 대해서는 각각 독립적인 연결을 유지합니다.
    /// 
    /// 사용 예:
    ///   auto ctrl = ConnectionManager::GetInstance().GetOrCreateConnection("192.168.1.100", 5000, 5000);
    ///   auto ctrl2 = ConnectionManager::GetInstance().GetOrCreateConnection("192.168.1.100", 5000, 5000); // 같은 소켓 공유
    ///   auto ctrl3 = ConnectionManager::GetInstance().GetOrCreateConnection("10.0.0.50", 6000, 5000);   // 다른 서버 - 별도 소켓
    /// </summary>
    class VMF_API ConnectionManager
    {
    public:
        static ConnectionManager& GetInstance();

        /// <summary>
        /// IP:Port에 해당하는 Controller를 반환합니다.
        /// 이미 존재하는 연결이 있으면 기존 Controller를 반환(소켓 공유),
        /// 없으면 새로 생성하여 반환합니다.
        /// </summary>
        std::shared_ptr<VC::Controller> GetOrCreateConnection(
            const std::string& ip, int port, int timeoutMs);

        /// <summary>
        /// 특정 연결을 해제합니다.
        /// </summary>
        void ReleaseConnection(const std::string& key);

        /// <summary>
        /// 모든 연결을 해제합니다.
        /// </summary>
        void DisconnectAll();

        /// <summary>
        /// IP:Port를 연결 키 문자열로 변환합니다.
        /// </summary>
        static std::string MakeConnectionKey(const std::string& ip, int port);

    private:
        ConnectionManager() = default;
        ~ConnectionManager();
        ConnectionManager(const ConnectionManager&) = delete;
        ConnectionManager& operator=(const ConnectionManager&) = delete;

        std::mutex m_mutex;
        // key("IP:Port") → weak_ptr<Controller>
        // 모든 참조가 사라지면(weak_ptr::lock() == nullptr) 자동 제거
        std::map<std::string, std::weak_ptr<VC::Controller>> m_connections;
    };
} // namespace VMF
