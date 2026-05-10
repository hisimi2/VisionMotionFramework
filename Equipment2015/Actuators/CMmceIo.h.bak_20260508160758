#pragma once

#include "IDio.h"

#include <vector>

// 프로젝트에 `byte` 타입이 정의되어 있지 않을 수 있으므로 안전하게 정의합니다.
#ifndef byte
typedef unsigned char byte;
#endif

class CMmceIo : public IDio
{
public:
	CMmceIo();
	virtual ~CMmceIo();

	// IDio 인터페이스 구현
	void out(int nChannel, bool bStatus) override;
	bool out(int nChannel) override;
	bool in(int nChannel) override;

private:
	// 총 바이트 수는 생성자에서 초기화합니다.
	int m_nTotalBytes;

	// I/O 스레드 관련 (std::thread/std::atomic/mutex 대신 Win32 API 사용 — v100 호환)
	void ioThreadFunc();           // 인스턴스 멤버 함수 (실제 스레드 진입점은 .cpp에서 static 함수로 연결)
	void startIoThread();
	void stopIoThread();

	std::vector<byte> UpdateHardwareAndWait(const std::vector<byte>& outputBuffer);

	HANDLE m_ioThreadHandle;       // 스레드 핸들
	DWORD  m_ioThreadId;           // 스레드 ID
	volatile LONG m_stopIoThread;  // 중지 플래그 (Interlocked 계열 사용 권장)

	// 공유 버퍼 보호: CRITICAL_SECTION 사용 (v100에서 사용 가능)
	CRITICAL_SECTION m_ioMutex;
	std::vector<byte> m_inputBuffer;
	std::vector<byte> m_outputBuffer;

	// 스레드 진입점 (Windows API)
	static DWORD WINAPI ThreadProc(LPVOID lpParameter);
};

