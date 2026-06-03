#pragma once

#include "IDio.h"

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

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

	// I/O 스레드 관련 (std::thread, std::atomic, std::mutex 사용)
	void ioThreadFunc();
	void startIoThread();
	void stopIoThread();

	std::vector<byte> UpdateHardwareAndWait(const std::vector<byte>& outputBuffer);

	std::thread        m_ioThread;
	std::atomic<bool>  m_stopIoThread;

	// 공유 버퍼 보호: std::mutex 사용
	std::mutex         m_ioMutex;
	std::vector<byte>  m_inputBuffer;
	std::vector<byte>  m_outputBuffer;
};

