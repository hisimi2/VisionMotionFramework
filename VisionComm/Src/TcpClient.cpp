#include "stdafx.h"

#include "TcpClient.h"
#include "IFramer.h"

#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <cstdlib> 

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

namespace VisionComm
{
	enum eSOCKET_INIT_TYPE
	{
		eSocketTypeClient = 0,
		eSocketTypeServer
	};

    static void WinsockCleanup()
    {
        ::WSACleanup();
    }

    static bool InitWinsock()
    {
        WSADATA wsa;
        const int rc = ::WSAStartup(MAKEWORD(2, 2), &wsa);
        if (rc != 0)
            return false;

        std::atexit(&WinsockCleanup);
        return true;
    }

    static bool EnsureWinsockInitialized()
    {
        static bool s_inited = InitWinsock();
        return s_inited;
    }

	struct TcpClient::Impl
	{
		TcpClient* m_parent;

		SOCKET m_sock;
		SOCKET m_listenSock;

		std::atomic<bool> m_running;
		std::atomic<bool> m_connected;

		RecvCallback m_recvCb;
		std::mutex m_cbMutex;      // 콜백 함수 보호
		std::mutex m_bufferMutex;  // m_partialBuffer 및 Framer 데이터 보호

		std::atomic<bool> m_callbackEnabled;

		std::thread m_recvThread;
		size_t m_recvBufferSize;
		std::vector<uint8_t> m_recvInternalBuffer; // RecvOnce 전용 재사용 버퍼

		// 패킷을 자르는 인터페이스
		IFramerPtr m_framer;

		int m_sendTimeoutMs;
		int m_recvTimeoutMs;

		Impl(TcpClient* parent)
			: m_parent(parent)
			, m_sock(INVALID_SOCKET)
			, m_listenSock(INVALID_SOCKET)
			, m_running(false)
			, m_connected(false)
			, m_callbackEnabled(false)
			, m_recvBufferSize(8192)
			, m_sendTimeoutMs(0)
			, m_recvTimeoutMs(0)
		{
			WSADATA wsa;
			WSAStartup(MAKEWORD(2, 2), &wsa);

			// 메모리 예외 방지를 위해 미리 공간 확보
			m_recvInternalBuffer.resize(m_recvBufferSize);
		}

		~Impl()
		{
			CleanupSocket();
			WSACleanup();
		}

		void CleanupSocket()
		{
			m_running = false;
			m_connected = false;

			// 버퍼 관련 작업 중일 수 있으므로 Lock
			std::lock_guard<std::mutex> lock(m_bufferMutex);

			if (m_sock != INVALID_SOCKET) {
				closesocket(m_sock);
				m_sock = INVALID_SOCKET;
			}

			if (m_listenSock != INVALID_SOCKET) {
				closesocket(m_listenSock);
				m_listenSock = INVALID_SOCKET;
			}
		}

		// 공통 소켓 설정 로직 분리
		void SetupSocketOptions(SOCKET s)
		{
			if (m_sendTimeoutMs > 0)
				setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char*)&m_sendTimeoutMs, sizeof(int));
			if (m_recvTimeoutMs > 0)
				setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&m_recvTimeoutMs, sizeof(int));

			// Keep-Alive 설정 추천 (연결 끊김 감지용)
			BOOL bKeepAlive = TRUE;
			setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char*)&bKeepAlive, sizeof(BOOL));
		}

		int CreateAndConnect(const char* ip, uint16_t port, int timeoutMs)
		{
#ifdef WIN32
			CleanupSocket();

			m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (m_sock == INVALID_SOCKET)
				return WSAGetLastError();

			SetupSocketOptions(m_sock);

			sockaddr_in addr;
			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);
			inet_pton(AF_INET, ip, &addr.sin_addr);

			// 비차단(Non-blocking) 연결 처리가 필요할 수 있으나 
			// 현재는 가장 기본적인 동기 connect 방식 유지
			int ret = connect(m_sock, (sockaddr*)&addr, sizeof(addr));

			if (ret == SOCKET_ERROR)
			{
				int err = WSAGetLastError();
				CleanupSocket();
				return err;
			}

			m_connected = true;
			m_running = true;
			m_callbackEnabled = true;

			return 0;
#else
			return -1;
#endif
		}

		int CreateAndListen(const char* ip, uint16_t port)
		{
#ifdef WIN32
			CleanupSocket();

			m_listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (m_listenSock == INVALID_SOCKET)
				return WSAGetLastError();

			BOOL opt = TRUE;
			setsockopt(m_listenSock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

			sockaddr_in addr;
			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);

			if (ip == nullptr)
				addr.sin_addr.s_addr = INADDR_ANY;
			else
				inet_pton(AF_INET, ip, &addr.sin_addr);

			if (bind(m_listenSock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
			{
				int err = WSAGetLastError();
				CleanupSocket();
				return err;
			}

			if (listen(m_listenSock, 5) == SOCKET_ERROR)
			{
				int err = WSAGetLastError();
				CleanupSocket();
				return err;
			}

			sockaddr_in clientAddr;
			int len = sizeof(clientAddr);

			// accept 대기 중 CleanupSocket이 호출되면 에러 반환됨
			m_sock = accept(m_listenSock, (sockaddr*)&clientAddr, &len);
			if (m_sock == INVALID_SOCKET)
			{
				int err = WSAGetLastError();
				CleanupSocket();
				return err;
			}

			SetupSocketOptions(m_sock);

			m_connected = true;
			m_running = true;
			m_callbackEnabled = true;

			return 0;
#else
			return -1;
#endif
		}
	};

    TcpClient::TcpClient()
        : m_pImpl(std::make_unique<Impl>(this)) // [수정] make_unique로 할당
    {
    }

    TcpClient::~TcpClient()
    {
        if (m_pImpl)
        {
            Disconnect();

            if (m_pImpl->m_recvThread.joinable())
            {
                m_pImpl->m_recvThread.join();
            }
        }
    }

	bool TcpClient::Connect(const char* ip, uint16_t port, int timeoutMs, int socketType)
	{
		if (!m_pImpl)
			return false;

		int ret;

		if (socketType == eSocketTypeClient)
			ret = m_pImpl->CreateAndConnect(ip, port, timeoutMs);
		else
			ret = m_pImpl->CreateAndListen(ip, port);

		if (ret == 0) {
			m_pImpl->m_connected = true;
		}

		return (ret == 0);
	}

    void TcpClient::Disconnect()
    {
        if (m_pImpl)
            m_pImpl->CleanupSocket();
    }

    bool TcpClient::IsConnected() const
    {
        if (m_pImpl)
            return m_pImpl->m_connected;
        return false;
    }

	void TcpClient::StartRecvThread()
	{
		m_pImpl->m_running = true;

		m_pImpl->m_recvThread = std::thread([this]()
		{
			std::vector<uint8_t> dummy;

			while (m_pImpl->m_running)
			{
				this->RecvOnce(dummy);

				// boost::this_thread::sleep -> std::this_thread::sleep_for
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		});
	}

	int TcpClient::RecvOnce(ByteVector& outBytes)
	{
		outBytes.clear();

#ifdef WIN32
		SOCKET sock = m_pImpl->m_sock;
		if (sock == INVALID_SOCKET) return -1;

		// 1. 내부 버퍼 크기 관리
		if (m_pImpl->m_recvInternalBuffer.empty()) {
			size_t sz = (m_pImpl->m_recvBufferSize > 0) ? (m_pImpl->m_recvBufferSize) : 8192;
			m_pImpl->m_recvInternalBuffer.resize(sz);
		}

		// 소켓 수신
		int n = ::recv(sock, reinterpret_cast<char*>(m_pImpl->m_recvInternalBuffer.data()),
			(int)m_pImpl->m_recvInternalBuffer.size(), 0);

		if (n > 0)
		{
			// 2. 버퍼 및 프레이머 보호를 위한 Lock
			std::lock_guard<std::mutex> lock(m_pImpl->m_bufferMutex);
			bool packetDispatched = false;
			int lastFrameSize = 0;

			if (m_pImpl->m_framer) // <--- IFramerPtr 사용
			{
				// 수신된 데이터를 프레이머에 전달
				ByteVector rawData(m_pImpl->m_recvInternalBuffer.begin(),
					m_pImpl->m_recvInternalBuffer.begin() + n);
				m_pImpl->m_framer->FeedBytes(rawData);

				// 프레이머가 패킷을 완성할 때까지 추출
				while (true)
				{
					auto framePtr = m_pImpl->m_framer->NextFrame();
					if (!framePtr) break;

					int fsize = static_cast<int>(framePtr->size());

					{
						std::lock_guard<std::mutex> cbLock(m_pImpl->m_cbMutex);
						if (m_pImpl->m_callbackEnabled && m_pImpl->m_recvCb)
						{
							// move into callback to avoid copy
							m_pImpl->m_recvCb(std::move(*framePtr));
							// framePtr moved; do not reuse
						}
					}

					// If callback wasn't called (or was disabled), return frame via outBytes
					// Note: when callback is called, the data has been moved into callback and outBytes remains empty
					if (!(m_pImpl->m_callbackEnabled && m_pImpl->m_recvCb)) {
						outBytes = std::move(*framePtr);
					}
					packetDispatched = true;
					lastFrameSize = fsize;
				}
			}
			else
			{
				// 프레이머가 없을 경우 기존 664 하드코딩 로직 유지 (Fallback)
				m_partialBuffer.insert(m_partialBuffer.end(),
					m_pImpl->m_recvInternalBuffer.begin(),
					m_pImpl->m_recvInternalBuffer.begin() + n);

				while (m_partialBuffer.size() >= 664)
				{
					std::vector<uint8_t> packet(m_partialBuffer.begin(), m_partialBuffer.begin() + 664);
					m_partialBuffer.erase(m_partialBuffer.begin(), m_partialBuffer.begin() + 664);

					bool calledCb = false;
					{
						std::lock_guard<std::mutex> cbLock(m_pImpl->m_cbMutex);
						if (m_pImpl->m_callbackEnabled && m_pImpl->m_recvCb) {
							m_pImpl->m_recvCb(std::move(packet));
							calledCb = true;
						}
					}

					if (!calledCb) {
						outBytes = std::move(packet);
					}
					packetDispatched = true;
					lastFrameSize = 664;
				}
			}

			return packetDispatched ? lastFrameSize : 0;
		}
		// ... 이하 에러 처리(n <= 0) 로직 동일
		if (n == 0)
		{
			// 연결 종료
			m_pImpl->m_connected = false;
			return 0;
		}
		else if (n < 0)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK || err == WSAEINTR)
				return 0;
			m_pImpl->m_connected = false;
			return -err;
		}
#endif
		return -1;
	}

	int TcpClient::Send(const ByteArray& data)
	{
		if (!m_pImpl || !m_pImpl->m_connected)
			return -1;

#ifdef WIN32

		const char* sendBuf =
			reinterpret_cast<const char*>(data.data());

		int totalSize = static_cast<int>(data.size());
		int totalSent = 0;

		while (totalSent < totalSize)
		{
			int sent = send(
				m_pImpl->m_sock,
				sendBuf + totalSent,
				totalSize - totalSent,
				0
			);

			if (sent == SOCKET_ERROR)
				return -WSAGetLastError();

			totalSent += sent;
		}

		return totalSent;

#else
		return -1;
#endif
	}

    void TcpClient::SetReceiveCallback(const RecvCallback& cb)
    {
        if (m_pImpl)
        {
            std::lock_guard<std::mutex> lock(m_pImpl->m_cbMutex);
            m_pImpl->m_recvCb = cb;
            m_pImpl->m_callbackEnabled = static_cast<bool>(cb);
        }
    }

    void TcpClient::SetRecvBufferSize(size_t sz)
    {
        if (m_pImpl) m_pImpl->m_recvBufferSize = sz;
    }

    void TcpClient::SetSocketTimeout(int sendMs, int recvMs)
    {
        if (m_pImpl) {
            m_pImpl->m_sendTimeoutMs = sendMs;
            m_pImpl->m_recvTimeoutMs = recvMs;
        }
    }

	void TcpClient::SetFramer(std::shared_ptr<IFramer> framer)
	{
		if (!m_pImpl) return;

		std::lock_guard<std::mutex> lock(m_pImpl->m_bufferMutex);
		m_pImpl->m_framer = framer;
	}
} 

