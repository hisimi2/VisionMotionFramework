#pragma once

#include "IDio.h"
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

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

	void ioThreadFunc();
	void startIoThread();
	void stopIoThread();

	std::vector<byte> UpdateHardwareAndWait(const std::vector<byte>& outputBuffer);

	std::thread        m_ioThread;
	std::atomic<bool>  m_stopIoThread;

	std::mutex         m_ioMutex;
	std::vector<byte>  m_inputBuffer;
	std::vector<byte>  m_outputBuffer;
};

