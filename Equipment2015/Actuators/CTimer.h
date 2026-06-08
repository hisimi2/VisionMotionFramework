#pragma once

#include <ctime> 

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
