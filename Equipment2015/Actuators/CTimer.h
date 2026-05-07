#pragma once

#include <ctime> // VS2010(v100) 호환을 위해 <chrono> 대신 <ctime> 사용

// CTimer: VS2010(v100) 호환 구현 (밀리초 단위)
class CTimer
{
public:
	// checkTimeMs: 밀리초 단위
	CTimer(long long checkTimeMs);
	bool isOver();
	void startTimer();

private:
	// 밀리초 단위로 저장
	unsigned long long m_checkTimeMs;
	std::clock_t m_start;
};