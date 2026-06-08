#include "stdafx.h"
#include "CMmceIo.h"
#include <algorithm>

CMmceIo::CMmceIo()
	: m_nTotalBytes(16)
	, m_stopIoThread(false)
{
	m_inputBuffer.resize(m_nTotalBytes, 0);
	m_outputBuffer.resize(m_nTotalBytes, 0);

	startIoThread();
}

CMmceIo::~CMmceIo()
{
	stopIoThread();
}

void CMmceIo::out(int nChannel, bool bStatus)
{
	if ((m_nTotalBytes * 8) <= nChannel)
	{
		TRACE("Buffer Number is too much high.");
		return;
	}

	unsigned int uIndex = static_cast<unsigned int>(nChannel / 8);
	unsigned int bit = 1u << (nChannel % 8);

	{
		std::lock_guard<std::mutex> lock(m_ioMutex);
		if (bStatus)
			m_outputBuffer[uIndex] = static_cast<byte>(m_outputBuffer[uIndex] | bit);
		else
			m_outputBuffer[uIndex] = static_cast<byte>(m_outputBuffer[uIndex] & ~bit);
	}
}

bool CMmceIo::out(int nChannel)
{
	if ((m_nTotalBytes * 8) <= nChannel)
	{
		return false;
	}

	unsigned int uIndex = static_cast<unsigned int>(nChannel / 8);
	unsigned int bit = 1u << (nChannel % 8);

	std::lock_guard<std::mutex> lock(m_ioMutex);
	return (m_outputBuffer[uIndex] & bit) != 0;
}

bool CMmceIo::in(int nChannel)
{
	if ((m_nTotalBytes * 8) <= nChannel)
	{
		return false;
	}

	unsigned int uIndex = static_cast<unsigned int>(nChannel / 8);
	unsigned int bit = 1u << (nChannel % 8);

	std::lock_guard<std::mutex> lock(m_ioMutex);
	return (m_inputBuffer[uIndex] & bit) != 0;
}

// 전용 I/O 스레드가 실행할 함수
void CMmceIo::ioThreadFunc()
{
	while (!m_stopIoThread)
	{
		std::vector<byte> localOutputBuffer;
		// 공유 버퍼에서 출력 데이터를 복사
		{
			std::lock_guard<std::mutex> lock(m_ioMutex);
			localOutputBuffer = m_outputBuffer;
		}

		// 느린 Blocking API 호출 (사용자 구현)
		std::vector<byte> localInputBuffer = UpdateHardwareAndWait(localOutputBuffer);

		// 공유 버퍼에 최신 입력 데이터를 씀
		{
			std::lock_guard<std::mutex> lock(m_ioMutex);
			m_inputBuffer = localInputBuffer;
		}

		Sleep(10); // I/O 스레드 자체의 주기
	}
}

std::vector<byte> CMmceIo::UpdateHardwareAndWait(const std::vector<byte>& outputBuffer)
{
	// 실제 하드웨어와 통신하는 코드 구현 필요
	std::vector<byte> dummyInputBuffer(outputBuffer.size(), 0);
	// TODO: 하드웨어 I/O 처리 구현
	Sleep(1);
	return dummyInputBuffer;
}

void CMmceIo::startIoThread()
{
	m_stopIoThread = false;

	if (m_ioThread.joinable())
		return;

	m_ioThread = std::thread(&CMmceIo::ioThreadFunc, this);
}

void CMmceIo::stopIoThread()
{
	m_stopIoThread = true;

	if (m_ioThread.joinable())
	{
		m_ioThread.join();
	}
}

