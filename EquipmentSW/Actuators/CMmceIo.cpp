#include "stdafx.h"
#include "CMmceIo.h"
#include <algorithm>

CMmceIo::CMmceIo()
	: m_nTotalBytes(16)
	, m_ioThreadHandle(NULL)
	, m_ioThreadId(0)
	, m_stopIoThread(0)
{
	// 버퍼 초기화
	m_inputBuffer.resize(m_nTotalBytes, 0);
	m_outputBuffer.resize(m_nTotalBytes, 0);

	// CRITICAL_SECTION 초기화
	InitializeCriticalSection(&m_ioMutex);

	startIoThread();
}

CMmceIo::~CMmceIo()
{
	stopIoThread();

	// CRITICAL_SECTION 해제
	DeleteCriticalSection(&m_ioMutex);
}

void CMmceIo::out(int nChannel, bool bStatus)
{
	// 범위 검사
	if ((m_nTotalBytes * 8) <= nChannel)
	{
		TRACE("Buffer Number is too much high.");
		return;
	}

	unsigned int uIndex = static_cast<unsigned int>(nChannel / 8);
	unsigned int bit = 1u << (nChannel % 8);

	EnterCriticalSection(&m_ioMutex);
	if (bStatus)
		m_outputBuffer[uIndex] = static_cast<byte>(m_outputBuffer[uIndex] | bit);
	else
		m_outputBuffer[uIndex] = static_cast<byte>(m_outputBuffer[uIndex] & ~bit);
	LeaveCriticalSection(&m_ioMutex);
}

bool CMmceIo::out(int nChannel)
{
	if ((m_nTotalBytes * 8) <= nChannel)
	{
		return false;
	}

	unsigned int uIndex = static_cast<unsigned int>(nChannel / 8);
	unsigned int bit = 1u << (nChannel % 8);

	EnterCriticalSection(&m_ioMutex);
	bool bReturnValue = (m_outputBuffer[uIndex] & bit) != 0;
	LeaveCriticalSection(&m_ioMutex);

	return bReturnValue;
}

bool CMmceIo::in(int nChannel)
{
	if ((m_nTotalBytes * 8) <= nChannel)
	{
		return false;
	}

	unsigned int uIndex = static_cast<unsigned int>(nChannel / 8);
	unsigned int bit = 1u << (nChannel % 8);

	EnterCriticalSection(&m_ioMutex);
	bool bReturnValue = (m_inputBuffer[uIndex] & bit) != 0;
	LeaveCriticalSection(&m_ioMutex);

	return bReturnValue;
}

// 전용 I/O 스레드가 실행할 함수
void CMmceIo::ioThreadFunc()
{
	// stop 플래그가 세트될 때까지 실행
	while (InterlockedCompareExchange(&m_stopIoThread, 0, 0) == 0)
	{
		std::vector<byte> localOutputBuffer;
		// 공유 버퍼에서 출력 데이터를 복사
		EnterCriticalSection(&m_ioMutex);
		localOutputBuffer = m_outputBuffer;
		LeaveCriticalSection(&m_ioMutex);

		// 느린 Blocking API 호출 (사용자 구현)
		std::vector<byte> localInputBuffer = UpdateHardwareAndWait(localOutputBuffer);

		// 공유 버퍼에 최신 입력 데이터를 씀
		EnterCriticalSection(&m_ioMutex);
		// 크기 불일치 처리: 안전하게 복사
		if (localInputBuffer.size() == m_inputBuffer.size())
			m_inputBuffer = localInputBuffer;
		else
		{
			// 필요시 크기를 맞춰 갱신
			m_inputBuffer.assign(localInputBuffer.begin(), localInputBuffer.end());
			if (m_inputBuffer.size() < static_cast<size_t>(m_nTotalBytes))
				m_inputBuffer.resize(m_nTotalBytes, 0);
		}
		LeaveCriticalSection(&m_ioMutex);

		Sleep(10); // I/O 스레드 자체의 주기
	}
}

std::vector<byte> CMmceIo::UpdateHardwareAndWait(const std::vector<byte>& outputBuffer)
{
	// 실제 하드웨어와 통신하는 코드 구현 필요
	// 예시: 입력 버퍼와 동일한 크기의 더미 데이터 반환
	std::vector<byte> dummyInputBuffer(outputBuffer.size(), 0);
	// TODO: 하드웨어 I/O 처리 구현
	Sleep(1); // 실제 API가 약간의 딜레이가 있다고 가정
	return dummyInputBuffer;
}

void CMmceIo::startIoThread()
{
	// stop 플래그 초기화 (0 = running)
	InterlockedExchange(&m_stopIoThread, 0);

	// 이미 스레드가 실행 중이면 무시
	if (m_ioThreadHandle != NULL)
		return;

	m_ioThreadHandle = CreateThread(
		NULL,
		0,
		CMmceIo::ThreadProc,
		this,
		0,
		&m_ioThreadId);

	// CreateThread 실패 시 NULL 리턴 -> 필요시 로그/처리 추가
}

void CMmceIo::stopIoThread()
{
	// 스레드 종료 신호 (1 = stop)
	InterlockedExchange(&m_stopIoThread, 1);

	if (m_ioThreadHandle != NULL)
	{
		// 스레드가 종료될 때까지 대기
		WaitForSingleObject(m_ioThreadHandle, INFINITE);
		CloseHandle(m_ioThreadHandle);
		m_ioThreadHandle = NULL;
		m_ioThreadId = 0;
	}
}

// Windows API 스레드 진입점
DWORD WINAPI CMmceIo::ThreadProc(LPVOID lpParameter)
{
	CMmceIo* pThis = static_cast<CMmceIo*>(lpParameter);
	if (pThis)
	{
		pThis->ioThreadFunc();
	}
	return 0;
}

