#include "stdafx.h"

#include "VisionController.h"
#include "VisionMsgDispatcher.h"
#include "ByteOrder.h"   
#include <queue>
// Boost 라이브러리
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/bind.hpp>


#include "IFramer.h"
#include "FixedLengthFramer.h"
namespace VisionCom
{
    // Member 구조체 정의
	struct VisionController::Impl
	{
		std::shared_ptr<ITransport> m_transport;
		std::shared_ptr<IFramer> m_framer;
		std::shared_ptr<IScheduler> m_scheduler;
		std::shared_ptr<ILogger>    m_logger;
		boost::atomic<bool>         m_running;
		boost::atomic<bool>         m_PacketReceived;
		boost::atomic<bool>         m_bStatusConnect;
		VisionMsgDispatcher*        m_dispatcher;

		std::shared_ptr<boost::thread> m_recvThread;

		// 추가: 수신 큐
		std::queue<ByteVector> m_packetQueue;
		boost::mutex m_queueMutex;

		Impl()
			: m_dispatcher(new VisionMsgDispatcher())
			, m_running(false)
			, m_PacketReceived(false)
		{

			m_transport = std::make_shared<VisionTcpClient>();

			// 1. 먼저 자식 타입으로 생성
			auto concreteFramer = std::make_shared<VisionCom::FixedLengthFramer>(664);

			m_framer = std::static_pointer_cast<VisionCom::IFramer>(concreteFramer);

			if (m_transport)
			{
				m_transport->SetFramer(m_framer);
			}
		}

		~Impl()
		{
			if (m_dispatcher)
			{
				delete m_dispatcher;
				m_dispatcher = NULL;
			}
		}

		// 
		void HandleTransportData(const ByteArray& raw)
		{
			if (!m_framer) return;

			try {
				m_framer->FeedBytes(raw);
				ByteVector frame;

				while (m_framer->NextFrame(frame)) {
					boost::lock_guard<boost::mutex> lock(m_queueMutex);
					m_packetQueue.push(frame); // 안전하게 큐에 저장
				}
			}
			catch (...) {
				if (m_logger) m_logger->Log("Error in HandleTransportData");
			}
		}


		void ProcessPackets()
		{
			std::queue<ByteVector> localQueue;
			{
				boost::lock_guard<boost::mutex> lock(m_queueMutex);
				std::swap(localQueue, m_packetQueue);
			}

			// 헤더 사이즈를 상수로 정의 (컴파일 타임에 결정됨)
			const size_t headerSize = sizeof(SECSPacketHeader);

			while (!localQueue.empty()) {
				const ByteVector& frame = localQueue.front();

				// 1. 최소 헤더 크기 체크 (하드코딩 12 대신 구조체 크기 사용)
				if (frame.size() >= headerSize) {

					// 2. 메모리 복사로 헤더 채우기 (가장 안전하고 깔끔한 방법)
					SECSPacketHeader header;
					std::memcpy(&header, frame.data(), headerSize);

					// 3. nLength 교정 (프레임 전체 크기 - 헤더 크기 = 순수 바디 크기)
					header.nLength = static_cast<int>(frame.size() - headerSize);

					// 4. 바디(Body) 데이터 추출
					ByteArray body;
					if (frame.size() > headerSize) {
						// 헤더 이후의 위치부터 끝까지 복사
						body.assign(frame.begin() + headerSize, frame.end());
					}

					// 5. 디스패치
					if (m_dispatcher) {
						m_dispatcher->Dispatch(header, body);
					}
				}
				localQueue.pop();
			}
		}
	};

    VisionController::VisionController() : m_pImpl(new Impl()) {}

    VisionController::~VisionController() 
    {
        StopReceiving();
        if(m_pImpl) 
        {
            delete m_pImpl;
            m_pImpl = NULL;
        }
    }

    void VisionController::SetTransport(std::shared_ptr<ITransport> transport)
    {
        if(m_pImpl) m_pImpl->m_transport = transport;
    }
    

    void VisionController::SetLogger(std::shared_ptr<ILogger> logger)
    {
        if(m_pImpl) m_pImpl->m_logger = logger;
    }

    void VisionController::SetScheduler(std::shared_ptr<IScheduler> scheduler)
    {
        if(m_pImpl)
        {
            m_pImpl->m_scheduler = scheduler;
            // 디스패처가 있으면 디스패처에도 스케줄러 전달
            if (m_pImpl->m_dispatcher) {
                m_pImpl->m_dispatcher->SetScheduler(scheduler);
            }
        }
    }

	bool VisionController::Initialize(char* szIp, int nPort, int nSocketType, int timeoutMs)
	{
		if (!m_pImpl)
			return false;

		// Transport 확인
		if (!m_pImpl->m_transport)
		{
			m_pImpl->m_transport = std::make_shared<VisionTcpClient>();
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


    void VisionController::Disconnect() 
    {
        StopReceiving();
        if(m_pImpl && m_pImpl->m_transport) 
        {
            m_pImpl->m_transport->Disconnect();
        }
    }

    bool VisionController::IsConnected() const 
    {
        if(m_pImpl && m_pImpl->m_transport)
            return m_pImpl->m_transport->IsConnected();
        return false;
    }

	VisionStatus VisionController::SendPacketAsync(const SECSPacket& pkt)
	{
		if (!m_pImpl) return VisionNotInitialized;

		Impl& impl = *m_pImpl;

		if (!impl.m_transport) return VisionNotInitialized;
		if (!impl.m_transport->IsConnected()) return VisionConnectionFailed;

		ByteArray payload = pkt.ToByteArray();

		int bytesSent = impl.m_transport->Send(payload);
		return (bytesSent > 0) ? VisionOK : VisionFailed;
	}

	void VisionController::StartReceiving()
	{
		if (!m_pImpl) return;

		Impl* pimpl = m_pImpl;

		if (pimpl->m_running) return;

		pimpl->m_running = true;

		if (pimpl->m_transport)
		{
			pimpl->m_transport->SetReceiveCallback(
				boost::bind(&VisionController::Impl::HandleTransportData, pimpl, _1)
			);

			pimpl->m_transport->StartRecvThread();
		}
	}

    void VisionController::StopReceiving()
    {
        if(!m_pImpl) return;

        Impl* pimpl = m_pImpl;
        pimpl->m_running = false;

        // transport 콜백 해제 (빈 함수로 설정)
        if (pimpl->m_transport)
        {
            pimpl->m_transport->SetReceiveCallback(TransportReceiveCallback());
        }
    }


    VisionMsgDispatcher& VisionController::GetDispatcher()
    {
        return *(m_pImpl->m_dispatcher);
    }


	void VisionController::PacketThread()
	{
		if (!m_pImpl) return;

		m_pImpl->ProcessPackets();  // 내부 Impl 호출
	}
}
