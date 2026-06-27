#include "stdafx.h"

#include "Controller.h"
#include "SecsMessageDispatcher.h"
#include "TcpClient.h"
#include "FixedLengthFramer.h"

#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>

namespace VC
{
	struct Controller::Impl
	{
		std::shared_ptr<ITransport> m_transport;
		std::shared_ptr<IFramer>    m_framer;
		std::shared_ptr<IScheduler> m_scheduler;
		std::shared_ptr<ILogger>    m_logger;
		
 		std::atomic<bool> m_running;
		std::atomic<bool> m_bStatusConnect;
		std::unique_ptr<SecsMessageDispatcher> m_dispatcher;

		// 수신 큐
		std::queue<ByteVector> m_packetQueue;
		std::mutex m_queueMutex;

		// 연결 키 (ConnectionManager 연동)
		std::string m_connectionKey;

		Impl()
			: m_dispatcher(std::make_unique<SecsMessageDispatcher>())
			, m_running(false)
            , m_bStatusConnect(false)
		{
			m_transport = std::make_shared<TcpClient>();

			//1. 먼저 자식 타입으로 생성
			auto concreteFramer = std::make_shared<FixedLengthFramer>(664);

			m_framer = std::static_pointer_cast<IFramer>(concreteFramer);

			if (m_transport)
			{
				m_transport->SetFramer(m_framer);
			}
		}

		~Impl() = default;

		// Transport already provides framed packets through the callback.
		// Accept frame as rvalue-reference and move into internal queue to avoid copies.
		void HandleTransportData(ByteVector&& frameData)
		{
			try {
				std::lock_guard<std::mutex> lock(m_queueMutex);
				m_packetQueue.push(std::move(frameData)); 
			}
			catch (...) {
				if (m_logger) m_logger->Log("Error in HandleTransportData");
			}
		}

		void ProcessPackets()
		{
			std::queue<ByteVector> localQueue;
			{
				std::lock_guard<std::mutex> lock(m_queueMutex);
				std::swap(localQueue, m_packetQueue);
			}

			// 헤더 사이즈를 상수로 정의 (컴파일 타임에 결정됨)
			const size_t headerSize = sizeof(SECSPacketHeader);

			while (!localQueue.empty()) {
				const ByteVector& frame = localQueue.front();

				//1. 최소 헤더 크기 체크 (하드코딩12 대신 구조체 크기 사용)
				if (frame.size() >= headerSize) {

					//2. 메모리 복사로 헤더 채우기 (가장 안전하고 깔끔한 방법)
					SECSPacketHeader header;
					std::memcpy(&header, frame.data(), headerSize);

					//3. nLength 교정 (프레임 전체 크기 - 헤더 크기 = 순수 바디 크기)
					header.nLength = static_cast<int>(frame.size() - headerSize);

					//4. 바디(Body) 데이터 추출
					ByteArray body;
					if (frame.size() > headerSize) {
						// 헤더 이후의 위치부터 끝까지 복사
						body.assign(frame.begin() + headerSize, frame.end());
					}

					//5. 디스패치
					if (m_dispatcher) {
						m_dispatcher->Dispatch(header, std::move(body));
					}
				}
				localQueue.pop();
			}
		}
	};

 
    Controller::Controller() : m_pImpl(std::make_shared<Impl>()) {}

    Controller::~Controller() 
    {
        StopReceiving();
    }

    void Controller::SetTransport(std::shared_ptr<ITransport> transport)
    {
        if(m_pImpl) m_pImpl->m_transport = transport;
    }
 
    void Controller::SetLogger(std::shared_ptr<ILogger> logger)
    {
        if(m_pImpl) m_pImpl->m_logger = logger;
    }

    void Controller::SetScheduler(std::shared_ptr<IScheduler> scheduler)
    {
        if(m_pImpl)
        {
            m_pImpl->m_scheduler = scheduler;
    
            if (m_pImpl->m_dispatcher) {
                m_pImpl->m_dispatcher->SetScheduler(scheduler);
            }
        }
    }

    bool Controller::Initialize(const char* szIp, int nPort, int nSocketType, int timeoutMs)
    {
	    if (!m_pImpl)
		    return false;

	    // Transport 확인
	    if (!m_pImpl->m_transport)
	    {
		    m_pImpl->m_transport = std::make_shared<TcpClient>();
	    }

	    // Connect 시도
	    bool connected = m_pImpl->m_transport->Connect(
		    szIp,
		    static_cast<uint16_t>(nPort),
		    timeoutMs,
		    nSocketType
	    );

	    if (!connected)
	    {
		    return false;
	    }

	    // 연결 상태 저장
	    m_pImpl->m_bStatusConnect = true;

	    return true;
    }

    void Controller::Disconnect() 
    {
        StopReceiving();
        if(m_pImpl && m_pImpl->m_transport) 
        {
            m_pImpl->m_transport->Disconnect();
        }
    }

    bool Controller::IsConnected() const 
    {
        if(m_pImpl && m_pImpl->m_transport)
        return m_pImpl->m_transport->IsConnected();

        return false;
    }

    Status Controller::SendPacketAsync(const SECSPacket& pkt)
    {
        if (!m_pImpl) return VisionNotInitialized;

        Impl& impl = *m_pImpl;

        if (!impl.m_transport) return VisionNotInitialized;
        if (!impl.m_transport->IsConnected()) return VisionConnectionFailed;

        ByteArray payload = pkt.ToByteArray();

        int bytesSent = impl.m_transport->Send(payload);
        return (bytesSent >0) ? VisionOK : VisionFailed;
    }

	void Controller::StartReceiving()
	{
		if (!m_pImpl) return;

		if (m_pImpl->m_running) return;

		m_pImpl->m_running = true;

		if (m_pImpl->m_transport)
		{
			// capture weak_ptr to avoid dangling pointer if controller is destroyed
			auto weakImpl = std::weak_ptr<Impl>(m_pImpl);

			m_pImpl->m_transport->SetReceiveCallback(
			[weakImpl](ByteVector&& data) {
				if (auto sp = weakImpl.lock()) {
					sp->HandleTransportData(std::move(data));
				}
			}
			);

			m_pImpl->m_transport->StartRecvThread();
		}
	}

    void Controller::StopReceiving()
    {
        if(!m_pImpl) return;

        m_pImpl->m_running = false;

        if (m_pImpl->m_transport)
        {
            m_pImpl->m_transport->SetReceiveCallback(TransportReceiveCallback());
        }
    }

    SecsMessageDispatcher& Controller::GetDispatcher()
    {
        return *(m_pImpl->m_dispatcher);
    }

	void Controller::PacketThread()
	{
		if (!m_pImpl) return;

		m_pImpl->ProcessPackets(); // 내부 Impl 호출
	}

    // ---- 연결 키 관리 ----
    void Controller::SetConnectionKey(const std::string& key)
    {
        if (m_pImpl)
        {
            m_pImpl->m_connectionKey = key;
        }
    }

    std::string Controller::GetConnectionKey() const
    {
        if (m_pImpl)
        {
            return m_pImpl->m_connectionKey;
        }
        return "";
    }

    bool Controller::IsReceiving() const
    {
        if (m_pImpl)
        {
            return m_pImpl->m_running.load();
        }
        return false;
    }

    bool Controller::HasPendingPackets() const
    {
        if (m_pImpl)
        {
            std::lock_guard<std::mutex> lock(m_pImpl->m_queueMutex);
            return !m_pImpl->m_packetQueue.empty();
        }
        return false;
    }
}

